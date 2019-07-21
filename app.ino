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
  */
void ISR__motion_detected( void )
{
    if( system_armed )
    {
        motion_detected = 1;
    }
}

/* ISR__override
* External interrupt called when 
* a switch pulls the pin to GND.
  */
void ISR__override( void )
{
    if( system_armed )
    {
        manual_override = 1;
    }
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
    Serial.println( "  ___                _      ___ _    _      ___          _      _   _         " );
    Serial.println( " | __|   _ __ _ _ _ ( )___ | __(_)__| |_   / __|_ __ _ _(_)_ _ | |_| |___ _ _ " );
    Serial.println( " |   / || / _` | ' \|/(_-< | _|| (_-< ' \  \__ \ '_ \ '_| | ' \| / / / -_) '_|" );
    Serial.println( " |_|_\\_, \__,_|_||_| /__/ |_| |_/__/_||_| |___/ .__/_| |_|_||_|_\_\_\___|_|  " );
    Serial.println( "      |__/                                     |_|                            " );
    Serial.println( "" );

    machine_state = st_idle;
}

/* st_idle
* A machine state to sit in waiting for motion to be detected.
 */
static void st_idle( void )
{
    if( 1 )
    {
        /* Motion detected! */
        machine_state = st_led_on;
    }
}

/* st_led_on
* A Machine state for turning ON the LED.
 */
static void st_led_on( void )
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50); 
    machine_state = st_led_off;
}

static void st_led_off( void )
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);    
    machine_state = st_led_on;
}

void loop()
{
    /* Run the current machine_state */
    machine_state();
}