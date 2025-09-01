class WeatherService
{
    static bool s_StormDeclared = false;
    static int  s_StormLastAlertMs = 0;
    static int  s_StormCooldownMs = 5 * 60 * 1000; // 5 minutes
    static bool s_Started;

    static void Start(float periodSec = 60.0)
    {
        if (s_Started) return;
        s_Started = true;
        ACHLog("[ACH][RADIO][DBG] WeatherService.Start scheduling Tick=" + periodSec + "s");
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Tick, (int)(periodSec * 1000), true);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Report, 5 * 60 * 1000, true);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SelfTest, 5000, false);
    }

    static void SelfTest()
    {
        string msg = "[RADIO 87.8] Weather system self-test";
        ACHLog("[ACH][RADIO][DBG] SelfTest firing: " + msg);
        RadioBroadcaster.Broadcast(msg);
    }

    static void Tick()
    {
        Weather w = g_Game.GetWeather();

        // Actuals & forecasts
        float ocA = w.GetOvercast().GetActual();
        float ocF = w.GetOvercast().GetForecast();
        float rnA = w.GetRain().GetActual();
        float rnF = w.GetRain().GetForecast();
        float fgA = w.GetFog().GetActual();
        float fgF = w.GetFog().GetForecast();

        // Wind
        float windSpeed = w.GetWindSpeed();
        vector wind = w.GetWind(); // [x, y, z]
        float windDirDeg = Math.Atan2(wind[2], wind[0]) * Math.RAD2DEG + 90.0;
        if (windDirDeg < 0) windDirDeg += 360.0;
        if (windDirDeg >= 360.0) windDirDeg -= 360.0;
        float wfSpeed, wfVar, wfGust;
        w.GetWindFunctionParams(wfSpeed, wfVar, wfGust);
        // Snowfall (map supports it via cfgWeather.xml)
        float sfA = w.GetSnowfall().GetActual();
        float sfF = w.GetSnowfall().GetForecast();

        string line1 = "[ACH][WEATHER] ocA=" + ocA + " ocF=" + ocF;
        line1 = line1 + " sfA=" + sfA + " sfF=" + sfF + " fgA=" + fgA;
        line1 = line1 +  " fgF=" + fgF + " windSpd=" + windSpeed + " wfSpeed=" + wfSpeed + " windDir=" + windDirDeg;
        WeatherLog(line1);

        float ocThresh = 0.98;                 // matches cfgWeather storm threshold
        bool inStormNow = (ocA >= ocThresh);
        bool stormComing = (ocF >= ocThresh) && ((ocF - ocA) >= 0.10);  // rising toward storm
        bool stormClearingSoon = inStormNow && (ocF < (ocThresh - 0.05)); // hysteresis to avoid flapping
        int nowMs = GetGame().GetTime();

        // entering (announce once, rate-limited)
        if (!s_StormDeclared && stormComing && (nowMs - s_StormLastAlertMs) > s_StormCooldownMs) {
            string warnIn = "[ACH][RADIO] Weather alert: Thunderstorm developing. Expect lightning and heavy clouds.";
            ACHLog(warnIn);
            RadioBroadcaster.Broadcast(warnIn);
            WeatherLog(warnIn);
            s_StormDeclared = true;
            s_StormLastAlertMs = nowMs;
        }

        // exiting (announce once while in storm and forecast drops below threshold)
        if (s_StormDeclared && stormClearingSoon && (nowMs - s_StormLastAlertMs) > s_StormCooldownMs) {
            string warnOut = "[ACH][RADIO] Weather update: Thunderstorm weakening. Lightning risk decreasing.";
            ACHLog(warnOut);
            RadioBroadcaster.Broadcast(warnOut);
            WeatherLog(warnOut);
            s_StormDeclared = false;
            s_StormLastAlertMs = nowMs;
        }
    }

    static void Report()
    {
        Weather w = g_Game.GetWeather();

        // Actuals
        float ocA = w.GetOvercast().GetActual();
        float sfA = w.GetSnowfall().GetActual();
        float fgA = w.GetFog().GetActual();

        // Wind: get API value and cross-check with vector magnitude
        float windMps_api = w.GetWindSpeed();
        vector wind = w.GetWind(); // [x,y,z]
        float windMps_vec = Math.Sqrt(wind[0]*wind[0] + wind[2]*wind[2]); // ignore Y

        float windMps = windMps_api;
        if (Math.AbsFloat(windMps_vec - windMps_api) > 2.0)  // big mismatch? trust vector
        {
            windMps = windMps_vec;
        }
        float windKmh = MpsToKmh(windMps);

        // Direction from vector
        float windDirDeg = Math.Atan2(wind[2], wind[0]) * Math.RAD2DEG + 90.0;
        if (windDirDeg < 0) windDirDeg += 360.0;
        if (windDirDeg >= 360.0) windDirDeg -= 360.0;
        string windDirTxt = GetCardinal(windDirDeg);

        // Phrases (use actuals for "Current conditions")
        string snowTxt  = SnowPhrase(sfA, windKmh);
        string fogTxt   = FogPhrase(fgA);
        string cloudTxt = OvercastPhrase(ocA);
        fogTxt.ToLower();
        cloudTxt.ToLower();

        // One clean line
        int windRound = Math.Round(windKmh);
        string lineA = "[ACHILLES RADIO] Current conditions: ";
        lineA = lineA + snowTxt;
	if (snowTxt == "Blizzard conditions")
        {
            lineA = lineA + ". ";
        }
        else
        {
            lineA = lineA + " with " + fogTxt + " and " + cloudTxt + ". ";
        }        
 	if (windRound < 3) 
        {
            lineA = lineA + "Calm wind conditions.";
        }
        else
        {
            string windPh = WindPhrase(windKmh); 
            lineA = lineA + windPh + windRound + " km/h from the " + windDirTxt + ".";
        }   
        ACHLog(lineA);
        RadioBroadcaster.Broadcast(lineA);
    }

    static string WindPhrase(float windKmh)
    {
        if (windKmh <= 7) return "Light air moving at ";
        else if (windKmh <= 12) return "Light breeze at ";
        else if (windKmh <= 18) return "Gentle breeze at ";
        else if (windKmh <= 24) return "Moderate breeze at ";
        else if (windKmh <= 34) return "Strong breeze moving at ";
        else if (windKmh <= 38) return "Winds at ";
        else if (windKmh <= 46) return "Strong winds moving at ";
        else if (windKmh <= 54) return "Near gale winds at ";
        else if (windKmh <= 64) return "Gale-force winds at ";
        else if (windKmh <= 72) return "Storm-force winds at ";
        else return "Strong storm-force winds at ";
    }

    static string OvercastPhrase(float ocA)
    {
        if (ocA >= 0.85) return "Overcast";           // 5
        if (ocA >= 0.65) return "Mostly cloudy";      // 4
        if (ocA >= 0.45) return "Partly cloudy";      // 3
        if (ocA >= 0.25) return "Scattered clouds";   // 2
        if (ocA >= 0.05) return "Few clouds";         // 1
        return "Clear skies";                         // 0
    }

    static string FogPhrase(float fgA)
    {
        if (fgA >= 0.80) return "Dense fog";          // 5
        if (fgA >= 0.60) return "Thick fog";          // 4
        if (fgA >= 0.40) return "Foggy";              // 3
        if (fgA >= 0.20) return "Mist";               // 2
        if (fgA >= 0.05) return "Haze";               // 1
        return "No fog";                              // 0
    }

    // 0..1 snowfall intensity -> 6-step forecast phrasing
    static string SnowPhrase(float sfA, float windKmh)
    {
        // Upgrade to blizzard if heavy snow + strong winds (common forecast convention)
        if (sfA >= 0.80 || (sfA >= 0.65 && windKmh >= 50.0)) return "Blizzard conditions";  // 6
        if (sfA >= 0.65) return "Heavy snow";      // 5
        if (sfA >= 0.50) return "Moderate snow";   // 4      
        if (sfA >= 0.35) return "Snow";            // 3  (steady/moderate)
        if (sfA >= 0.20) return "Light snow";      // 2
        if (sfA >= 0.05) return "Flurries";        // 1
        return "Clear";                            // 0
    }

    static float MpsToKmh(float mps)
    {
        return mps * 3.6;   // 1 m/s = 3.6 km/h
    }

    static string GetCardinal(float deg)
    {
        string dirs[8] = {"N","NE","E","SE","S","SW","W","NW"};
        int index = Math.Round(deg / 45.0);
        while (index >= 8) {
            index -= 8;
        }
        while (index < 0) {
            index += 8;
        }
        return dirs[index];
    }


    static string Pad2(int n)
    {
        string s = "" + n;
        if (n < 10) s = "0" + s;
        return s;
    }

    static void WeatherLog(string line)
    {
        int y, mo, da, h, mi;
        GetGame().GetWorld().GetDate(y, mo, da, h, mi); // out-params form

        string YY = "" + y;
        string MO = Pad2(mo);
        string DA = Pad2(da);
        string HH = Pad2(h);
        string MM = Pad2(mi);

        string final = "[" + YY + "-" + MO + "-" + DA + " " + HH + ":" + MM + "] " + line;

        FileHandle fh = OpenFile("$profile:weather.log", FileMode.APPEND);
        if (fh != 0)
        {
            FPrintln(fh, final);
            CloseFile(fh);
        }
    }
}
