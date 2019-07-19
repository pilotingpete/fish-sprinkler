void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

/* State Machine */
typedef void (*fp)( void );
fp machine_state = wakeup; /* Initial machine_state */


/* Machine states */
static void wakeup( void )
{
    machine_state = led_on;
}

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