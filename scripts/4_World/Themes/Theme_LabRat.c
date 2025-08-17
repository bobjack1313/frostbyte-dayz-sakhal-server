// LabRat Mix of doc and professional
class Theme_LabRat
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Coat + slacks
        InventoryUtil.ReplaceSlotServer(p, "Body", "LabCoat", 0.25, 0.40);

        autoptr array<string> pants = {
            "SlacksPants_Beige","SlacksPants_Black","SlacksPants_Blue","SlacksPants_Brown",
            "SlacksPants_DarkGrey","SlacksPants_Khaki","SlacksPants_LightGrey","SlacksPants_White"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.45, 0.70);

        // Dress shoes (ensure these exist in your server/mod set)
        autoptr array<string> shoes = { "DressShoes_Beige","DressShoes_Black","DressShoes_Brown","DressShoes_Sunburst","DressShoes_White" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        // 88% chance glasses
        if (Math.RandomFloat01() < 0.88)
        {
            autoptr array<string> frames = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, frames.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        // ~2-use alcohol on QB 1
        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); InventoryUtil.SetWorn(alc); InventoryUtil.BindQuickbar(p, alc, 1); }

        // Damaged BoneKnife on QB 0 (no bandage)
        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
