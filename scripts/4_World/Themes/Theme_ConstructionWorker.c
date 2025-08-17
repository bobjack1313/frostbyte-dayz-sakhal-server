// Construction Worker - vest, boots, hard hat
class Theme_ConstructionWorker
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        autoptr array<string> shirts = {
            "TShirt_Beige","TShirt_Black","TShirt_Blue","TShirt_Green","TShirt_Grey",
            "TShirt_OrangeWhiteStripes","TShirt_Red","TShirt_RedBlackStripes","TShirt_White"
        };
        autoptr array<string> pants  = { "Jeans_Brown","Jeans_Black","Jeans_BlueDark","Jeans_Green","Jeans_Gray" };
        autoptr array<string> boots  = { "WorkingBoots_Beige","WorkingBoots_Brown","WorkingBoots_Green","WorkingBoots_Grey","WorkingBoots_Yellow" };
        autoptr array<string> hardHat = {
            "ConstructionHelmet_Blue","ConstructionHelmet_Lime","ConstructionHelmet_Orange",
            "ConstructionHelmet_Red","ConstructionHelmet_White","ConstructionHelmet_Yellow"
        };

        string shirtPick = shirts[Math.RandomInt(0, shirts.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", shirtPick, 0.45, 0.75);

        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.45, 0.70);

        string hatPick = hardHat[Math.RandomInt(0, hardHat.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Headgear", hatPick, 0.30, 0.55);

        InventoryUtil.ReplaceSlotServer(p, "Vest", "ReflexVest", 0.25, 0.52);

        // Apply boots (you had the pool but weren’t using it)
        string bootPick = boots[Math.RandomInt(0, boots.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", bootPick, 0.45, 0.70);

        // Bandage (pristine) on quickbar 1
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        // Two stones
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }
    }
}
