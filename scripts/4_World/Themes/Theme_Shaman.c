class Theme_Shaman
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // hard-strip torso/legs
        InventoryUtil.ClearSlotServer(p, "Body");
        InventoryUtil.ClearSlotServer(p, "Legs");

        // shoes (random color)
        autoptr array<string> shamanShoes = { "LeatherShoes_Beige","LeatherShoes_Black","LeatherShoes_Brown","LeatherShoes_Natural" };
        string shoePick = shamanShoes[Math.RandomInt(0, shamanShoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.50, 0.75);

        // wolf headdress
        InventoryUtil.ReplaceSlotServer(p, "Headgear", "Headdress_Wolf", 0.65, 0.80);

        // bag — pristine-ish
        autoptr array<string> shamanBag = { "SlingBag_Black","SlingBag_Brown","SlingBag_Gray" };
        string bagPick = shamanBag[Math.RandomInt(0, shamanBag.Count())];
        EntityAI bag = InventoryUtil.ReplaceSlotServer(p, "Back", bagPick, 0.65, 0.90);

        // contents (prefer bag; fallback to clothes)
        EntityAI band = null;
        EntityAI knife = null;

        if (bag && bag.GetInventory())
        {
            band  = bag.GetInventory().CreateInInventory("BandageDressing");
            knife = bag.GetInventory().CreateInInventory("BoneKnife");
        }
        if (!band)  band  = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (!knife) knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");

        if (band)  { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }
        if (knife) { InventoryUtil.SetRange(knife, 0.65, 0.65); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
