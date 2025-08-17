// Doctor: matching scrubs set (Blue/Green/White) + matching surgical gloves,
// random dress shoes. Items: DisinfectantAlcohol (~2 uses) + BoneKnife (damaged).
// No bandage. Common items come from GiveCommonItems(...).
class Theme_Doctor
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // pick a scrubs color and match all pieces
        autoptr array<string> cols = { "Blue","Green","White" };
        string col = cols[Math.RandomInt(0, cols.Count())];

        // scrubs + gloves + mask (worn-ish)
        InventoryUtil.ReplaceSlotServer(p, "Headgear",   "MedicalScrubsHat_"   + col, 0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Body",   "MedicalScrubsShirt_" + col, 0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Legs",   "MedicalScrubsPants_" + col, 0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Gloves", "SurgicalGloves_"     + col, 0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Mask",   "SurgicalMask",              0.45, 0.70);

        // random dress shoes (worn) — trim to known-good if you’re not using a mod
        autoptr array<string> shoes = { "DressShoes_Beige","DressShoes_Black","DressShoes_Brown" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        // 64% chance to wear glasses
        if (Math.RandomFloat01() < 0.64)
        {
            autoptr array<string> frames = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        // items: ~2-use alcohol + damaged bone knife (no bandage)
        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); InventoryUtil.SetRandom(alc); InventoryUtil.BindQuickbar(p, alc, 1); }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
