
/* GPIO pin defines */
#define DRV__WATER      2   /* Water Solenoid Drive*/
#define DRV__AIR        4   /* Air Solenoid Drive */
#define _MON__OVERRIDE   7  /* Switch Input - Active Low */
#define MON__MOTION     8   /* PIR Motion Detector */

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
    pinMode( _MON__OVERRIDE, INPUT_PULLUP );

}

/* State Machine */
typedef void (*fp)( void );
fp machine_state = st_wakeup; /* Init the state machine */


/* st_wakeup
* A machine state for setting default parameters.
*
 */
static void wakeup( void )
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
*
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
*
 */
static void st_led_on( void )
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50); 
    machine_state = led_off;
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