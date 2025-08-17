// Prisoner: jacket + pants + cap (very damaged → worn), shoes worn (random color),
// Bandage + 2x SmallStone + HandcuffsLocked. No knife.
class Theme_Prisoner
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Clothes
        InventoryUtil.ReplaceSlotServer(p, "Body", "PrisonUniformJacket", 0.25, 0.75);
        InventoryUtil.ReplaceSlotServer(p, "Legs", "PrisonUniformPants",  0.25, 0.75);
        InventoryUtil.ReplaceSlotServer(p, "Headgear", "PrisonerCap",         0.25, 0.75);

        // Shoes (worn, random color)
        autoptr array<string> prisonShoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown",
            "AthleticShoes_Green","AthleticShoes_Grey"
        };
        string shoePick = prisonShoes[Math.RandomInt(0, prisonShoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        // Bandage (pristine) on quickbar 1
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        // HandcuffsLocked (random health)
        EntityAI cuffs = InventoryUtil.PutInAnyClothesCargo(p, "HandcuffsLocked");
        if (cuffs) InventoryUtil.SetRandom(cuffs);

        // Two stones (random health)
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }
    }
}
