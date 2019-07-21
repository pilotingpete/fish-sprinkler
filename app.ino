void setup()
{
    /* Add serial support. */
    Serial.begin(9600);

    /* GPIO setup. */
    pinMode(LED_BUILTIN, OUTPUT);
}

/* State Machine */
typedef void (*fp)( void );
fp machine_state = wakeup; /* Init the state machine */


/* wakeup
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

    machine_state = led_on;
}

/* led_on
* A Machine state for turning ON the LED.
*
 */
static void led_on( void )
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50); 
    machine_state = led_off;
}

static void led_off( void )
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);    
    machine_state = led_on;
}

void loop()
{
    /* Run the current machine_state */
    machine_state();
}