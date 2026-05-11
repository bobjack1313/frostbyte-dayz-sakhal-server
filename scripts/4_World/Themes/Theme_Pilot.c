class Theme_Pilot
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Hat
        InventoryUtil.ReplaceSlotServer(p, "Headgear", "OfficerHat", 0.75, 0.95);

        // Jacket
        autoptr array<string> jacket = { "PilotJacket_Black", "PilotJacket_Yellow" };
        string jacketPick = jacket[Math.RandomInt(0, jacket.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", jacketPick, 0.50, 0.75);

        // Pants
        autoptr array<string> pants = {
            "SlacksPants_Blue","SlacksPants_Black","SlacksPants_Brown","SlacksPants_Beige","SlacksPants_Khaki","SlacksPants_LightGrey","SlacksPants_DarkGrey"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.50, 0.75);

        // dress shoes
        autoptr array<string> shoes = { "DressShoes_Beige","DressShoes_Brown","DressShoes_Black" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        // Aviators
        InventoryUtil.ReplaceSlotServer(p, "Eyewear", "AviatorGlasses", 0.65, 0.80);

        // items: ~2-use alcohol + steak knife (no bandage)
        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); InventoryUtil.SetRandom(alc); InventoryUtil.BindQuickbar(p, alc, 1); }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
