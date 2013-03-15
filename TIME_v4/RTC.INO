
void put_time(void)
{
    now = RTC.now();
    
    int hour = now.hour();
    int minute = now.minute();
    
    clear_screen();
    
    if (hour >= 10) {
        put_char(0, '0' + hour/10);   // First digit
        put_char(4, '0' + hour%10);   // Second digit
    }
    else put_char(4, '0'+hour);

    put_char(10, '0' + minute/10);   // First digit
    put_char(14, '0' + minute%10);   // Second digit
    
    set_pixel(8, 1);
    set_pixel(8, 3);
}
