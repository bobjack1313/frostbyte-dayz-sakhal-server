class RadioQueue
{
    // Queue file lives under $profile to avoid permissions fights
    static string Path()
    {
        return "$profile:radio_queue.tsv";
    }

    // Read all lines; return messages whose timestamp <= now; rewrite future lines back to file
    static array<string> RunDue()
    {
        array<string> due = new array<string>();
        array<string> keep = new array<string>();

        int now = GetGame().GetTime() / 1000; // epoch seconds, server uptime based

        // Open for READ; if missing, nothing to do
        FileHandle f = OpenFile(Path(), FileMode.READ);
        if (f)
        {
            string line;
            while (FGets(f, line) > 0)
            {
                line.Trim();
                if (line == "" || line.Substring(0,1) == "#") continue;

                TStringArray cols = new TStringArray();
                line.Split("\t", cols);

                // Expected: ts \t tag \t message
                if (cols.Count() < 3)
                {
                    // If legacy 2-column format: ts \t message
                    if (cols.Count() == 2)
                    {
                        int ts2 = cols[0].ToInt();
                        string msg2 = cols[1];
                        if (now >= ts2) due.Insert(msg2);
                        else keep.Insert(line);
                    }
                    else
                    {
                        // malformed, drop it
                    }
                    continue;
                }

                int ts = cols[0].ToInt();
                string tag = cols[1];
                string msg = cols[2];

                if (now >= ts)
                {
                    // Optionally decorate by tag; for now, just emit message
                    // if (tag == "TONE") { /* could prefix or play tone via mod */ }
                    due.Insert(msg);
                }
                else
                {
                    keep.Insert(line);
                }
            }
            CloseFile(f);
        }

        // Rewrite remaining entries atomically
        FileHandle w = OpenFile(Path(), FileMode.WRITE);
        if (w)
        {
            foreach (string row : keep) { FPrintln(w, row); }
            CloseFile(w);
        }

        return due;
    }
}
