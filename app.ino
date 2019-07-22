/* Global Vars */
volatile uint8_t manual_override = 0;
volatile uint8_t motion_detected = 0;
volatile uint8_t system_armed = 0;
uint16_t cycle_count = 0;

/* Cycle times */
uint32_t rearm_ms = 10000;
uint16_t water_fill_ms = 2500;
uint16_t air_actuation_ms = 300;

/* GPIO pin defines */
#define DRV__WATER      7   /* Water Solenoid Drive*/
#define DRV__AIR        8   /* Air Solenoid Drive */
#define _MON__OVERRIDE  2   /* Switch Input - Active Low */
#define MON__MOTION     3   /* PIR Motion Detector */

#define DO_DEBUG
#ifdef DO_DEBUG
    #define DEBUG( time_stamp, s ) {Serial.print( "Debug: " );  \
                                    Serial.print( time_stamp ); \
                                    Serial.print( " ms, " );       \
                                    Serial.println( s );}
#else
    #define DEBUG( t, s ) 
#endif

void setup()
{
    /* Add serial support. */
    Serial.begin(9600);

    /* Digital Outputs */
    pinMode( LED_BUILTIN, OUTPUT ); /* Pin 13 Arduino board LED */
    pinMode( DRV__AIR, OUTPUT );    
    pinMode( DRV__WATER, OUTPUT );

    /* Digital Inputs */
    pinMode( MON__MOTION, INPUT );
    attachInterrupt(digitalPinToInterrupt( MON__MOTION ), ISR__motion_detected, RISING );
    pinMode( _MON__OVERRIDE, INPUT_PULLUP );
    attachInterrupt(digitalPinToInterrupt( _MON__OVERRIDE ), ISR__override, FALLING );
}

/* ISR__motion_detected
* External interrupt called when
* the PIR sensor is activated.
*
* @warn This also disarms the system,
* so you will need to manually arm 
* it again to retrigger.
  */
void ISR__motion_detected( void )
{
    if( system_armed )
    {
        //system_armed = 0;
        //motion_detected = 1;
    }
}

/* ISR__override
* External interrupt called when 
* a switch pulls the pin to GND.
* 
  */
void ISR__override( void )
{
    manual_override = 1;
}

/* is_override
* Tests if the manual override
* flag is set.
* This will also clear the flag.
* 
* @return 1 if set, else 0.
  */
uint8_t is_override( void )
{
    const uint8_t retval = manual_override == 1 ? 1 : 0;
    manual_override = 0;
    return retval;
}

/* clear_inputFlags
* Clear the flags from the PIR & override ISR.
* This might be a noisy environment with the 
* solenoids so could be good to flear the flags
* after solenoid actuation and before checking them.
  */
void clear_input_flags( void )
{
    manual_override = 0;
    motion_detected = 0;
}

/* is_motion
* Tests if the motion detector
* flag is set.
* This will also clear the flag.
* 
* @return 1 if set, else 0.
  */
uint8_t is_motion( void )
{
    const uint8_t retval = motion_detected == 1 ? 1 : 0;
    motion_detected = 0;
    return retval;
}

/* sol_air_on
* Turns ON the air solenoid.
 */
static void sol_air_on( void )
{
    digitalWrite( DRV__AIR, HIGH );
}

/* sol_air_off
* Turns OFF the air solenoid.
 */
static void sol_air_off( void )
{
    digitalWrite( DRV__AIR, LOW );
}

/* sol_water_on
* Turns ON the water solenoid.
 */
static void sol_water_on( void )
{
    digitalWrite( DRV__WATER, HIGH );
}

/* sol_water_off
* Turns OFF the water solenoid.
 */
static void sol_water_off( void )
{
    digitalWrite( DRV__WATER, LOW );
}

/* State Machine */
typedef void (*fp)( void );
fp machine_state = st_wakeup; /* Init the state machine */


/* st_wakeup
* A machine state for setting default parameters.
 */
static void st_wakeup( void )
{
    /* Print welcome banner. */
    Serial.println( "" );
    Serial.println( "  _____                   _       ______ _     _        _____            _       _    _           " );
    Serial.println( " |  __ \\                ( )     |  ____(_)   | |      / ____|          (_)     | |  | |          " );
    Serial.println( " | |__) |   _  __ _ _ __ |/ ___  | |__   _ ___| |__   | (___  _ __  _ __ _ _ __ | | _| | ___ _ __ " );
    Serial.println( " |  _  / | | |/ _` | '_ \\ / __| |  __| | / __| '_ \\ \\__ \\| '_\\| '__| | '_\\| |/ / |/ _\\ '__|" );
    Serial.println( " | |\\\\ |_| | (_| | | | |\\__ \\| |    | \\_\\ | | |  ____) | |_) | |  | | | | |   <| |  __/ |   " );
    Serial.println( " |_| \\\\__, \\__,_|_| |_| |___/ |_|    |_|___/_| |_| |_____/| .__/|_|  |_|_| |_|_\\\\_\\___|_|   " );
    Serial.println( "         __/ |                                               | |                                  " );
    Serial.println( "        |___/                                                |_|                                  " );
    Serial.println( "" );

    clear_input_flags();

    DEBUG( millis(), "State: st_water_fill" );
    machine_state = st_water_fill;
}

/* st_water_fill
* A Machine state for filling the water chamber.
 */
static void st_water_fill( void )
{
    sol_water_on();
    delay( (uint32_t)water_fill_ms ); 
    sol_water_off();
    delay( 250 );

    clear_input_flags();
    system_armed = 1;

    DEBUG( millis(), "State: st_idle" );
    machine_state = st_idle;
}


/* st_idle
* A machine state to sit in while waiting for 
* motion to be detected.
 */
static void st_idle( void )
{
    if( is_motion() || is_override() )
    {
        /* Motion detected! */
        cycle_count++;
        Serial.print( "This is actuation count: ");
        Serial.println( cycle_count );

        DEBUG( millis(), "State: st_actuate" );
        machine_state = st_actuate;
    }
}

/* st_actuate
* A machine state to actuate the air solenoid.
* This it the main feature.
 */
static void st_actuate( void )
{
    sol_air_on();
    delay( (uint32_t)air_actuation_ms );
    sol_air_off();
    delay( 250 );

    DEBUG( millis(), "State: st_rearm_delay" );
    machine_state = st_rearm_delay;
}


/* st_rearm_delay
* A machine state to delay before rearming the 
* water feature.
 */
static void st_rearm_delay( void )
{
    clear_input_flags();

    uint32_t start_time = millis();

    while( millis() - start_time < (uint32_t)rearm_ms )
    {
        /* Bail early if an override has been requested. */
        if( is_override() )
        {
            Serial.println( "Bailing early on the rearm delay" );
            break;
        }
    }

    DEBUG( millis(), "State: st_water_fill" );
    machine_state = st_water_fill;
}


void loop()
{
    /* Run the current machine_state */
    machine_state();
}