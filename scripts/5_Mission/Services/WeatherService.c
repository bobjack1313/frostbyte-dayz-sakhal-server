class WeatherService
{
    static bool s_Started;

    static void Start(float periodSec = 60.0)
    {
        if (s_Started) return;
        s_Started = true;
        // schedule Tick every periodSec seconds
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Tick, (int)(periodSec * 1000), true);
    }

    static void Tick()
    {
        Weather w = g_Game.GetWeather();
        float wind = w.GetWindSpeed();
        Print(string.Format("[ACH][WEATHER] wind=%.1f", wind));
        if (wind > 20)
        {
            // minimal log to prove it runs
            FileHandle fh = OpenFile("$profile:radio_events.log", FileMode.APPEND);
            if (fh) { FPrintln(fh, string.Format("WEATHER wind=%.1f", wind)); CloseFile(fh); }
        }
    }
}
