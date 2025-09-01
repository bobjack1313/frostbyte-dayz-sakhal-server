class PlayerCommands
{
    ref set<string> g_AdminUIDs = new set<string>();
    ref map<string, bool> m_God = new map<string, bool>();
    ref map<string, Particle> m_GodFX = new map<string, Particle>();
    
    void LoadAdmins()
    {
ACHLog("Entering LoadADminsd");
        string path = "$profile:admins.xml";
        FileHandle fh = OpenFile(path, FileMode.READ);
        if (!fh) {
            Print("[ADMINS] admins.xml not found at " + path);
            return;
        }

        string line;
        while (FGets(fh, line) > 0) {
            // look for: <admin id="76561198..."/>
            int i = line.IndexOf("id=\"");
            if (i >= 0) {
                int start = i + 4;
                int j = line.IndexOfFrom(start, "\"");
                if (j > start) {
                    string uid = line.Substring(start, j - start);
                    uid.TrimInPlace();
                    if (uid != "") {
                        g_AdminUIDs.Insert(uid);
                        Print("[ADMINS] loaded UID=" + uid);
                    }
                }
            }
        }
        CloseFile(fh);
    }

    bool IsAdmin(PlayerIdentity ident)
    {
        if (!ident) return false;
        string uid = ident.GetId();
        foreach (string a : g_AdminUIDs) {
            if (a == uid) return true;
        }
        return false;
    }

    // --- Command router ---
    void HandleChatCommand(PlayerIdentity ident, string raw)
    {
	ACHLog("Entering Handle Chat Command");
        string cmd, args;
        ParseCommand(raw, cmd, args);
	ACHLog("After Parse - cmd=" + cmd);
        switch (cmd)
        {
            case "help":
		ACHLog("Case help");
                if (!IsAdmin(ident))
                {
		    ACHLog("IS NOT Admin");
                    Reply(ident, "Commands: !help, !rules, !server, !time, !players, !report <msg>");
                }                
                else
                {
                    Print("IS Admin");
                    Reply(ident, "Commands: !help, !rules, !server, !time, !players, !report <msg>, where, tp, clone, godmode ");
                }
                break;

            case "rules":
                Reply(ident, "Rules: No cheating. No hate speech. Be cool. Detailed rules on our Discord.");
                break;

            case "server":
                Reply(ident, "Server: Community PvE/PvP mix | Restarts every 10h | Map: Sakhal | Build: Vanilla+Scripts");
                break;

            case "time":
            {
                int year, month, day, hour, min;
                GetGame().GetWorld().GetDate(year, month, day, hour, min);
                Reply(ident, string.Format("Server date/time: %1-%2-%3 %4:%5", year, month, day, hour, min));
                break;
            }

            case "players":
            {
                // array<Man> players = new array<Man>();
                // GetGame().GetPlayers(players);
                // Reply(ident, string.Format("Players online: %1", players.Count()));
                // break;

                array<PlayerBase> ps;
                GetOnlinePlayers(ps);

                // Build a compact list: "#:Name (UIDlast4)"
                string line = string.Format("Players online: %1", ps.Count());
                Reply(ident, line);

                if (ps.Count() > 0) {
                    string list = "";
                    int idx = 1;
                    foreach (PlayerBase p : ps)
                    {
                        PlayerIdentity pi = p.GetIdentity();

                        string uid = "unknown";
                        if (pi) {
                            uid = pi.GetId();
                        }

                        string tail = uid;
                        if (uid.Length() >= 4) {
                            tail = uid.Substring(uid.Length() - 4, 4);
                        }

                        string name = "unknown";
                        if (pi) {
                            name = pi.GetName();
                        }

                        list = list + idx + ":" + name + " (" + tail + ")  ";
                        idx++;
                    }
                    Reply(ident, list.Trim());
                }
                break;
            }

            case "report":
            {
                if (args == "")
                {
                    Reply(ident, "Usage: !report <what happened>");
                    break;
                }

                // Timestamp (YYYY-MM-DD HH:MM)
                int y, mo, d, h, mi;
                GetGame().GetWorld().GetDate(y, mo, d, h, mi);
                PlayerBase pb = GetPlayerByIdentity(ident);
                vector pos;
                if (pb) {
                    pos = pb.GetPosition();
                } else {
                    pos = "0 0 0";
                }
                string grid2 = FormatGrid(pos);
                string rline = "[REPORT] " + y + "-" + mo + "-" + d + " " + h + ":" + mi;
                rline = rline + " uid=" + ident.GetId();
                rline = rline + " name=\"" + ident.GetName() + "\"";
                rline = rline + " pos=(" + Math.Round(pos[0]) + "," + Math.Round(pos[1]) + "," + Math.Round(pos[2]) + ")";
                rline = rline + " grid=" + grid2;
                rline = rline + " msg=" + args;

                Print(rline);                 // RPT
                GetGame().AdminLog(rline);    // admin log (if enabled)

                Reply(ident, "Thanks. Your report was logged for admins.");
                break;
            }

            case "where":
            {
                if (!IsAdmin(ident))
                {
                    Reply(ident, "Admin only.");
                    break;
                }
                PlayerBase pb2 = GetPlayerByIdentity(ident);
                if (!pb2)
                {
                    Reply(ident, "Could not locate your player.");
                    break;
                }

                vector vecP = pb2.GetPosition(); // x,y,z
                string grid3 = FormatGrid(vecP);
                Reply(ident, string.Format("Position: x=%1 y=%2 z=%3 | Grid: %4", Math.Round(vecP[0]), Math.Round(vecP[1]), Math.Round(vecP[2]), grid3));
                break;
            }

            case "tp":
            {
                // gate: only admins (logged in) can use; swap this to your own check if needed
                if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { Reply(ident, "Server only."); break; }
                if (!IsAdmin(ident))
                {
                    Reply(ident, "Admin only.");
                    break;
                }

                PlayerBase me = GetPlayerByIdentity(ident);
                if (!me) { Reply(ident, "Player not found."); break; }

                // Usage: !tp <x> <z>  OR  !tp <playerNamePart>
                if (args == "") { Reply(ident, "Usage: !tp <x> <z>  or  !tp <player>"); break; }

                TStringArray parts = new TStringArray;
                args.Split(" ", parts);

                // declare holders first
                float x, z;

                if (parts.Count() == 2 && IsFloat(parts.Get(0), x) && IsFloat(parts.Get(1), z))
                {
                    if (TeleportToXZ(me, x, z)) {
                        Reply(ident, "Teleported to " + x + "," + z);
                    } else {
                        Reply(ident, "Teleport failed (bad coords?).");
                    }
                    break;
                }

                // Teleport to another player’s position
                PlayerBase target = FindPlayerByName(args);
                if (!target) { Reply(ident, "Player not found."); break; }

                vector tpos = target.GetPosition();
                if (TeleportToXZ(me, tpos[0] + 0.5, tpos[2] + 0.5)) {
                    Reply(ident, "Teleported to " + target.GetIdentity().GetName());
                } else {
                    Reply(ident, "Teleport failed.");
                }
                break;
            }

            case "clone":
            {
                if (!IsAdmin(ident)) 
                {
                    Reply(ident, "Admin only. Use #login first.");
                    break;
                }

                PlayerBase me2 = GetPlayerByIdentity(ident);
                if (!me2) { Reply(ident, "Player not found."); break; }

                EntityAI inHands = me2.GetHumanInventory().GetEntityInHands();
                if (!inHands) { Reply(ident, "Hold the item you want to clone."); break; }

                string ty = inHands.GetType();
                ItemBase src = ItemBase.Cast(inHands);

                // Try put clone in inventory
                EntityAI clone = me2.GetInventory().CreateInInventory(ty);
                if (!clone) {
                    // Fall back: drop at feet
                    vector vp = me2.GetPosition(); vp[1] = GetGame().SurfaceY(vp[0], vp[2]) + 0.2;
                    clone = EntityAI.Cast(GetGame().CreateObjectEx(ty, vp, ECE_PLACE_ON_SURFACE));
                }
                if (!clone) { Reply(ident, "Clone failed."); break; }

                // Copy health
                float health = inHands.GetHealth("", "");
                clone.SetHealth("", "", health);

                ItemBase dst = ItemBase.Cast(clone);
                if (src && dst && src.HasQuantity() && dst.HasQuantity()) {
                    dst.SetQuantity(src.GetQuantity());
                }

                Reply(ident, "Cloned " + ty);
                break;
            }

            case "godmode":
            {
                if (!IsAdmin(ident))
                {
                    Reply(ident, "Admin only. Use #login first.");
                    break;
                }
                PlayerBase godMe = GetPlayerByIdentity(ident);
                if (!godMe)
                {
                    Reply(ident, "Player not found.");
                    break;
                }
                string uid2 = ident.GetId();
                bool on = !IsGod(uid2);
                SetGod(godMe, on);
                if (on)
		{
	            Reply(ident,  "Godmode ON.");
                }
                else
                {
                    Reply(ident, "Godmode OFF.");
                }
                break;
            }

            default:
                Reply(ident, "Unknown command. Try !help");
                break;
        }
    }

    // Return all connected PlayerBase objects
    void GetOnlinePlayers(out array<PlayerBase> outPlayers)
    {
        outPlayers = new array<PlayerBase>();
        array<Man> tmp = new array<Man>();
        GetGame().GetPlayers(tmp);
        foreach (Man m : tmp) {
            PlayerBase pb = PlayerBase.Cast(m);
            if (pb) outPlayers.Insert(pb);
        }
    }

    PlayerBase GetPlayerByIdentity(PlayerIdentity ident)
    {
        if (!ident) return null;
        string uid = ident.GetId();

        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man m : players) {
            PlayerBase pb = PlayerBase.Cast(m);
            if (!pb) continue;
            PlayerIdentity pi = pb.GetIdentity();
            if (pi && pi.GetId() == uid) {
                return pb;
            }
        }
        return null;
    }

    // --- Helpers ---
    bool IsFloat(string s, out float result)
    {
        result = 0.0;
        s.TrimInPlace();
        if (s == "") return false;

        // Basic numeric check
        int dots = 0;
        for (int i = 0; i < s.Length(); i++) {
            string ch = s.Substring(i, 1);
            if (ch == ".") {
                dots++;
                if (dots > 1) return false;
                continue;
            }
            if (ch == "-" || ch == "+") {
                if (i != 0) return false; // only allow sign at start
                continue;
            }
            if (ch < "0" || ch > "9") return false;
        }

        result = s.ToFloat();
        return true;
    }

    bool IsInt(string s, out int result)
    {
        result = 0;
        s.TrimInPlace();
        if (s == "") return false;

        for (int i = 0; i < s.Length(); i++) {
            string ch = s.Substring(i, 1);
            if (ch == "-" || ch == "+") {
                if (i != 0) return false;
                continue;
            }
            if (ch < "0" || ch > "9") return false;
        }

        result = s.ToInt();
        return true;
    }

    bool IsGod(string uid) 
    { 
        return m_God.Contains(uid) && m_God.Get(uid);
    }

    void SetGod(PlayerBase p, bool on)
    {
        if (!p) return;
        p.SetAllowDamage(!on); // off = invulnerable
	    string uid = "";	
        if (p.GetIdentity())
        {
            uid = p.GetIdentity().GetId();
        }
        if (uid == "") return;
        if (on) m_God.Set(uid, true); else m_God.Remove(uid);
    }

    void SetGodFX(PlayerBase p, bool on)
    {
        if (!p || !p.GetIdentity()) return;
        string uid = p.GetIdentity().GetId();

        if (on) {
            if (!m_GodFX.Contains(uid)) {
                Particle fx = Particle.PlayOnObject(ParticleList.CAMP_NORMAL_SMOKE, p); // or CAMP_SMALL_SMOKE
                if (fx) m_GodFX.Set(uid, fx);
            }
        } else {
            if (m_GodFX.Contains(uid)) {
                Particle fx2 = m_GodFX.Get(uid);
                if (fx2) fx2.Stop();
                m_GodFX.Remove(uid);
            }
        }
    }

    PlayerBase FindPlayerByName(string needle)
    {
        needle.ToLower();
        array<Man> all = new array<Man>();
        GetGame().GetPlayers(all);

        foreach (Man m : all) {
            PlayerBase p = PlayerBase.Cast(m);
            if (!p) continue;

            PlayerIdentity id = p.GetIdentity();
            if (!id) continue;

            string nm = id.GetName();
            nm.ToLower();

            if (nm.Contains(needle)) {
                return p;
            }
        }
        return null;
    }

    bool TeleportToXZ(PlayerBase p, float x, float z)
    {
        float y = GetGame().SurfaceY(x, z);
        if (y <= 0) return false;
        vector pos = Vector(x, y + 0.2, z); // slight lift to avoid clipping
        p.SetDirection(p.GetDirection());   // preserve facing (optional)
        p.SetPosition(pos);
        return true;
    }

    void ParseCommand(string raw, out string cmd, out string args)
    {
        Print("In Parse Command");
        // strip leading '/' or '!'
        string s = raw.Substring(1, raw.Length() - 1);
        int sp = s.IndexOf(" ");
        Print("sp= "+ sp);

        if (sp > 0) {
            string c = s.Substring(0, sp);
            c.TrimInPlace();
            c.ToLower();
            cmd = c;

            string a = s.Substring(sp + 1, s.Length() - sp - 1);
            a.TrimInPlace();
            args = a;
        } else {
            string c2 = s;
            c2.TrimInPlace();
            c2.ToLower();
            cmd = c2;
            args = "";
        }
    }

    void Reply(PlayerIdentity ident, string text)
    {
	Print("In Reply");
        if (!ident || text == "") return;

        // find their PlayerBase
        PlayerBase p = GetPlayerByIdentity(ident);
        if (p) {
            p.MessageStatus(text);   // private status message (bottom-left)
            // alternatives: p.MessageFriendly(text); p.MessageImportant(text);
        }

        // optional echo for debugging
        Print("[CHAT-REPLY to " + (ident.GetName()) + "] " + text);
        GetGame().AdminLog("[CHAT-REPLY to " + (ident.GetName()) + "] " + text);
    }

    string FormatGrid(vector p)
    {
        // 1km squares typical “map grid” style for quick callouts
        int gx = Math.Floor(p[0] / 1000);
        int gz = Math.Floor(p[2] / 1000);
        return string.Format("%1-%2", gx, gz);
    }

    bool TryParseFloat(string s, out float value)
    {
        s.TrimInPlace();
        value = 0.0;

        if (s == "") return false;

        int dots = 0;
        for (int i = 0; i < s.Length(); i++) {
            string ch = s.Substring(i, 1);
            if (ch == ".") {
                dots++;
                if (dots > 1) return false;
                continue;
            }
            if (ch == "-" || ch == "+") {
                if (i != 0) return false; // sign only at start
                continue;
            }
            if (ch < "0" || ch > "9") return false;
        }

        value = s.ToFloat(); // no params in Enforce
        return true;
    }

    bool TryParseInt(string s, out int value)
    {
        s.TrimInPlace();
        value = 0;

        if (s == "") return false;

        for (int i = 0; i < s.Length(); i++) {
            string ch = s.Substring(i, 1);
            if (ch == "-" || ch == "+") {
                if (i != 0) return false; // sign only at start
                continue;
            }
            if (ch < "0" || ch > "9") return false;
        }

        value = s.ToInt(); // no params in Enforce
        return true;
    }
}
