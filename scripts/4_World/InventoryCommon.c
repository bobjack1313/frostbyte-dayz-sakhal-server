// scripts/4_World/InventoryCommon.c
class CommonLoadout
{
    static void GiveRandomArmband(PlayerBase p)
    {
        if (!p) return;
        autoptr array<string> colors = {
            "Armband_White","Armband_Yellow","Armband_Orange","Armband_Red",
            "Armband_Green","Armband_Blue","Armband_Black"
        };
        string pick = colors[Math.RandomInt(0, colors.Count())];
        // remove any existing armband (server-authoritative)
        EntityAI existing = p.FindAttachmentBySlotName("Armband");
        if (existing) {
            GetGame().ObjectDelete(existing);
        }

        int slotId = InventorySlots.GetSlotIdFromString("Armband");

        // try direct slot attach first
        EntityAI a = p.GetInventory().CreateAttachmentEx(pick, slotId);
        if (!a) {
            // fallback: create in inventory, then server-take into slot
            EntityAI inv = p.GetInventory().CreateInInventory(pick);
            if (inv) {
                p.GetInventory().TakeEntityAsAttachmentEx(InventoryMode.SERVER, inv, slotId);
                a = p.FindAttachmentBySlotName("Armband");
            }
        }

        if (a) {
            InventoryUtil.SetPristine(a);
        } else {
            ACHLog("[ACH][Armband] attach failed for " + pick);
        }
    }

    static void GiveCommonItems(PlayerBase p) { GiveCommonItemsEx(p, false); }
    static void GiveCommonItemsEx(PlayerBase p, bool skipArmband)
    {
        if (!p) return;

        // Random chemlight
        autoptr array<string> chems = {
            "Chemlight_White","Chemlight_Yellow","Chemlight_Green","Chemlight_Red" 
        };
        EntityAI chem = InventoryUtil.PutInAnyClothesCargo(p, chems[Math.RandomInt(0, chems.Count())]);
        ItemBase chemIB = ItemBase.Cast(chem);
        if (chemIB) { InventoryUtil.SetRandom(chemIB); InventoryUtil.BindQuickbar(p, chemIB, 2); }

        // Damaged UnknownFoodCan
        ItemBase can = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "UnknownFoodCan"));
        if (can) { can.SetHealth01("", "", Math.RandomFloatInclusive(0.45, 0.75)); }

        // Optional armband
        if (!skipArmband) GiveRandomArmband(p);

        // Everyone gets a radio (powered if possible)
        ItemBase radio = ItemBase.Cast(p.GetInventory().CreateInInventory("PersonalRadio"));
        if (radio)
        {
            radio.SetHealth01("", "", 1.0);

            // best-effort: attach a 9V (no-op if already attached or slot is full)
            ItemBase.Cast(radio.GetInventory().CreateAttachment("Battery9V"));

            // Power it via energy manager (paramless; safe across versions)
            auto em = radio.GetCompEM();
            if (em)
            {
                if (!em.IsSwitchedOn() && em.CanSwitchOn())
                    em.SwitchOn();
            }

            p.SetQuickBarEntityShortcut(radio, 3);
        }
    }
}
