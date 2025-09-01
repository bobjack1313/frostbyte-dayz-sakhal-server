// scripts/5_Mission/Services/RadioBroadcaster.c
class RadioBroadcaster
{
    // Optional: encode target freq in message like "[RADIO 87.8] ..."
    static float TARGET_FREQ = 87.8;

    static void Broadcast(string msg)
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        int total = players.Count();
        int sent = 0;
        ACHLog("[ACH][RADIO][DBG] attempting send to " + total + " players: \"" + msg + "\"");

        foreach (Man m : players)
        {
            PlayerBase p = PlayerBase.Cast(m);
            if (!p) continue;
            bool ok = HasWorkingRadio(p);
            ACHLog("[ACH][RADIO][DBG] player=" + p.GetIdentity().GetName() + " radio_ok=" + ok);

            if (ok)
            {
                p.MessageStatus(msg);
                sent++;
            }
        }
        ACHLog("[ACH][RADIO][DBG] delivered=" + sent + " / " + total);
    }

    // Minimal, safe check: radio exists and is powered + working.
    // TODO( tuning ): If your PersonalRadio script exposes tuned freq/channel, add that check here.
    static bool HasWorkingRadio(PlayerBase p /*, float freq, string msg */)
    {
        array<EntityAI> inv = new array<EntityAI>();
        p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inv);
        foreach (EntityAI e : inv)
        {
            PersonalRadio r = PersonalRadio.Cast(e);
            if (!r) continue;

            ComponentEnergyManager em = r.GetCompEM();
            if (em && em.IsSwitchedOn() && em.IsWorking())
            {
                // If/when API allows, check tuning here:
                // float tuned = r.GetTunedFrequency(); if (Math.Abs(tuned - freq) > 0.05) continue;
                return true;
            }
        }
        return false;
    }
}
