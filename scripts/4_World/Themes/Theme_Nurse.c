// Nurse: females = NurseDress_(White/Blue) + HipPack_Medical; males = matching scrubs set.
// Shoes: random sneakers (worn). Glasses: 42% chance (Thick/Thin).
// Items: DisinfectantAlcohol (~2 uses) + BoneKnife (damaged). No bandage.
// NOTE: Common armband/chemlight/UnknownFoodCan come from GiveCommonItems(..).
class Theme_Nurse
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        if (male)
        {
            autoptr array<string> cols = { "Blue","Green","White" };
            string col = cols[Math.RandomInt(0, cols.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Body", "MedicalScrubsShirt_" + col, 0.35, 0.60);
            InventoryUtil.ReplaceSlotServer(p, "Legs", "MedicalScrubsPants_" + col, 0.35, 0.60);
            InventoryUtil.ReplaceSlotServer(p, "Headgear", "MedicalScrubsHat_" + col, 0.35, 0.60);
        }
        else
        {
            autoptr array<string> dcols = { "White","Blue" };
            string dcol = dcols[Math.RandomInt(0, dcols.Count())];
            // ensure no pants under the dress
            InventoryUtil.ClearSlotServer(p, "Legs");
            InventoryUtil.ReplaceSlotServer(p, "Body", "NurseDress_" + dcol, 0.65, 0.75);
            // keep if your server/mod supports this slot+item
            InventoryUtil.ReplaceSlotServer(p, "Hips", "HipPack_Medical", 0.45, 0.70);
        }

        autoptr array<string> nurseShoes = { "Sneakers_Black","Sneakers_Gray","Sneakers_Green","Sneakers_Red","Sneakers_White" };
        string shoePick = nurseShoes[Math.RandomInt(0, nurseShoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        if (Math.RandomFloat01() < 0.42)
        {
            autoptr array<string> frames = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, frames.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); InventoryUtil.SetWorn(alc); InventoryUtil.BindQuickbar(p, alc, 1); }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
