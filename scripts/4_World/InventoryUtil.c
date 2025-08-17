// scripts/4_World/InventoryUtil.c
class InventoryUtil
{
    // ---- health helpers (same names as original helpers) ----
    static void SetPristine(EntityAI e) { if (e) e.SetHealth01("", "", 1.0); }
    static void SetWorn(EntityAI e)     { if (e) e.SetHealth01("", "", Math.RandomFloat(0.50, 0.69)); }
    static void SetLow(EntityAI e)      { if (e) e.SetHealth01("", "", Math.RandomFloat(0.20, 0.39)); }
    static void SetRandom(EntityAI e)   { if (e) e.SetHealth01("", "", Math.RandomFloat(0.31, 0.80)); }
    static void SetRange(EntityAI e, float lo, float hi)
    {
        if (!e) return;
        float v = Math.Clamp(Math.RandomFloat(lo, hi), 0.0, 1.0);
        e.SetHealth01("", "", v);
    }

    // ---- clear a slot ----
    static void ClearSlotServer(PlayerBase p, string slotName)
    {
        if (!p) return;
        EntityAI e = p.FindAttachmentBySlotName(slotName);
        if (e) GetGame().ObjectDelete(e);
    }

    static EntityAI ReplaceSlotServer(PlayerBase p, string slotName, string cls, float hMin, float hMax)
    {
        if (!p || cls == "") return null;

        //ACHLog("[ACH][ReplaceSV] slot=" + slotName + " cls=" + cls);

        // 1) Kill whatever's there (server-authoritative)
        EntityAI existing = p.FindAttachmentBySlotName(slotName);
        if (existing) {
            GetGame().ObjectDelete(existing);
        }

        int slotId = InventorySlots.GetSlotIdFromString(slotName);

        // 2) Try direct attach first
        EntityAI a = p.GetInventory().CreateAttachmentEx(cls, slotId);
        if (!a) {
            a = p.GetInventory().CreateAttachment(cls);
        }

        // 3) If still not there, create in inventory and server-take into slot
        if (!p.FindAttachmentBySlotName(slotName)) {
            EntityAI inv = p.GetInventory().CreateInInventory(cls);
            if (inv) {
                p.GetInventory().TakeEntityAsAttachmentEx(InventoryMode.SERVER, inv, slotId);
            } else {
                ACHLog("[ACH][ReplaceSV] CreateInInventory failed for " + cls);
            }
        }

        // 4) Verify final state only
        EntityAI check = p.FindAttachmentBySlotName(slotName);
        if (check && check.GetType() == cls) {
            check.SetHealth01("", "", Math.RandomFloat(hMin, hMax));
            return check;
        }

        return null;
    }

    // ---- cargo placement in any worn clothing/backpack; original behavior ----
    static EntityAI PutInAnyClothesCargo(PlayerBase p, string type)
    {
        // wrapper to preserve old call sites
        return PutInAnyClothingCargo(p, type);
    }

    static EntityAI PutInAnyClothingCargo(PlayerBase p, string itemClass)
    {
        array<string> slots = {"Body","Legs","Back","Vest","Hips","Feet"};
        for (int i = 0; i < slots.Count(); i++)
        {
            EntityAI clothing = p.FindAttachmentBySlotName(slots[i]);
            if (clothing && clothing.GetInventory())
            {
                EntityAI item = clothing.GetInventory().CreateInInventory(itemClass);
                if (item)
                {
                    //ACHLog("[ACH][Cargo] placed " + itemClass + " in " + slots[i]);
                    return item;
                }
            }
        }
        ACHLog("[ACH][Cargo] failed to place " + itemClass);
        return null;
    }

    // ---- quickbar ----
    static void BindQuickbar(PlayerBase p, EntityAI item, int idx)
    {
        if (p && item) p.SetQuickBarEntityShortcut(item, idx);
    }
}
