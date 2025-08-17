// ===== 3_Game: Name + Logging utils (moved from old init.c) =====

// Persisted name cache
autoptr map<string, string> g_NameCache = new map<string, string>();

string NameCachePath()
{
    return "$profile:name_cache.csv";
}

void LoadNameCache()
{
    FileHandle f = OpenFile(NameCachePath(), FileMode.READ);
    if (!f) return;

    string line;
    while (FGets(f, line) > 0) {
        line.Trim();
        if (line == "" || (line.Length() > 0 && line.Substring(0, 1) == "#")) continue;

        TStringArray kv = new TStringArray;
        line.Split(",", kv);
        if (kv.Count() >= 2) {
            string val = kv[1];
            // strip surrounding quotes if present
            val.Replace("\"", "");
            g_NameCache.Set(kv[0], val);
        }
    }
    CloseFile(f);
}

void SaveNameKV(string uid, string name)
{
    if (uid == "" || name == "" || name == "Survivor") {
        ACHLog("[NAMECACHE] Skipped saving: uid=" + uid + " name=" + name);
        return;
    }
    g_NameCache.Set(uid, name);
    FileHandle f = OpenFile(NameCachePath(), FileMode.APPEND);
    if (f) {
        string row = uid + "," + name;
        FPrintln(f, row);
        CloseFile(f);
        ACHLog("[NAMECACHE] Saved row: " + row);
    }
}

// Simple append-only log
void ACHLog(string s)
{
    FileHandle fh = OpenFile("$profile:ach.log", FileMode.APPEND);
    if (fh != 0)
    {
        // Keep it simple; date retrieval is present but unused in original
        string t; GetGame().GetWorld().GetDate(NULL, NULL, NULL, NULL, NULL);
        FPrintln(fh, s);
        CloseFile(fh);
    }
}

// Radio/event log with timestamp
void LogRadio(string line)
{
    FileHandle fh = OpenFile("$profile:radio_events.log", FileMode.APPEND);
    if (fh != 0)
    {
        int y, mo, d, h, mi;
        GetGame().GetWorld().GetDate(y, mo, d, h, mi);
        string ts = string.Format("%1-%2-%3 %4:%5", y, mo, d, h, mi);
        FPrintln(fh, string.Format("%1 [RADIO_EVENT] %2", ts, line));
        CloseFile(fh);
    }
}

// Round to 100m grid, as in your original
string GridRef(vector p)
{
    int gx = Math.Round(p[0] / 100) * 100;
    int gz = Math.Round(p[2] / 100) * 100;
    return string.Format("%1,%2", gx, gz);
}

string SafeName(PlayerBase p)
{
    if (!p) return "Unknown";

    PlayerIdentity id = p.GetIdentity();
    string nm = "Unknown";
    string uid = "";

    if (id)
    {
        nm  = id.GetName();
        uid = id.GetPlainId();
    }

    // Try cached name if current is empty/generic
    string cached;
    if ((nm == "" || nm == "Survivor") && uid != "" && g_NameCache && g_NameCache.Find(uid, cached))
    {
        nm = cached;
    }

    // Last-ditch: append short UID
    if (nm == "" || nm == "Survivor")
    {
        int len = uid.Length();
        int start = 0;
        if (len > 6) start = len - 6;

        string tail = "";
        if (len > 0) tail = uid.Substring(start, len - start);

        if (tail != "")
        {
            nm = "Survivor[" + tail + "]";
        }
        else
        {
            nm = "Survivor";
        }
    }

    return nm;
}

// Killer labeling (player/infected/animal/entity/environment)
string KillerLabel(Object killerObj, PlayerBase victim = null)
{
    if (!killerObj) return "Environment";

    PlayerBase kp = PlayerBase.Cast(killerObj);
    if (kp) {
        if (victim && kp == victim) return "Suicide";
        return SafeName(kp);
    }

    DayZInfected zi = DayZInfected.Cast(killerObj);
    if (zi) return "Infected";

    AnimalBase ab = AnimalBase.Cast(killerObj);
    if (ab) return ab.GetType();

    EntityAI ent = EntityAI.Cast(killerObj);
    if (ent) return ent.GetType();

    return killerObj.GetType();
}
