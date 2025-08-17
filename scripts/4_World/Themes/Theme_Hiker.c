// Hiker, similar to base, but hoodie, better shoes added
class Theme_Hiker
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Hoodie
        autoptr array<string> hoodies = { "Hoodie_Black","Hoodie_Blue","Hoodie_Brown","Hoodie_Green","Hoodie_Grey","Hoodie_Red" };
        string hoodiePick = hoodies[Math.RandomInt(0, hoodies.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", hoodiePick, 0.40, 0.60);

        // Pants
        autoptr array<string> pants = { "CanvasPants_Beige","CanvasPants_Blue","CanvasPants_Grey","CanvasPants_Red","CanvasPants_Violet" };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.45, 0.75);

        // Hiking shoes (sturdier)
        autoptr array<string> shoes = { "HikingBootsLow_Beige","HikingBootsLow_Black","HikingBootsLow_Blue","HikingBootsLow_Grey" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.35, 0.50);

        // Items: pristine bandage on QB 1
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
