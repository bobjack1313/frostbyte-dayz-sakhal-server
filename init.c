#include "$CurrentDir:./mpmissions/dayzOffline.sakhal/scripts/__includes.c"

void main()
{
    Hive ce = CreateHive();
    if (ce) ce.InitOffline();

    int year, month, day, hour, minute;
    int reset_month = 2, reset_day = 1;
    GetGame().GetWorld().GetDate(year, month, day, hour, minute);

    if ((month == reset_month) && (day < reset_day))
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    else if ((month == reset_month + 1) && (day > reset_day))
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    else if ((month < reset_month) || (month > reset_month + 1))
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
}

Mission CreateCustomMission(string path)
{
    return new CustomMission(); // implemented in scripts/5_Mission/CustomMission.c
}
