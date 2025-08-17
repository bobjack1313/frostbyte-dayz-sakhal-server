class MissionTime
{
    static void Start()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Tick, 5000, true);
    }

    static void Tick()
    {
        int now = GetGame().GetTime() / 1000;
        FileHandle f = OpenFile("$profile:mission_now.txt", FileMode.WRITE);
        if (f) { FPrintln(f, now.ToString()); CloseFile(f); }
    }
}
