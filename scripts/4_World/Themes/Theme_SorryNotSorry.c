// Sorry Not Sorry (Naked Guy): no clothes, worn sneakers (random), worn 20-slot sling bag (random).
// Bag contains: worn SteakKnife, worn BrassKnuckles_Dull, worn Heatpack.
// Chemlight + UnknownFoodCan + armband are handled by your common step.
class Theme_SorryNotSorry
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // 1) Hard strip (server-authoritative)
        autoptr array<string> stripSlots = { "Body","Legs","Headgear","Back" };
        for (int i = 0; i < stripSlots.Count(); i++)
        {
            InventoryUtil.ClearSlotServer(p, stripSlots[i]);
        }

        // 2) Picks
        autoptr array<string> nakedShoes = { "Sneakers_Black","Sneakers_Gray","Sneakers_Green","Sneakers_Red","Sneakers_White" };
        autoptr array<string> slingBags  = { "SlingBag_Black","SlingBag_Brown","SlingBag_Gray" };

        string shoes = nakedShoes[Math.RandomInt(0, nakedShoes.Count())];
        string bag   = slingBags[Math.RandomInt(0, slingBags.Count())];

        // 3) Wearables
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoes, 0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Back", bag,   0.45, 0.70);
        // Intentionally leave Body/Legs/Head empty

        // 4) Put kit into bag cargo, then quickbar
        EntityAI backpack = p.FindAttachmentBySlotName("Back");
        if (backpack && backpack.GetInventory())
        {
            EntityAI knife = backpack.GetInventory().CreateInInventory("SteakKnife");
            if (knife) InventoryUtil.SetRange(knife, 0.45, 0.70);

            EntityAI knux = backpack.GetInventory().CreateInInventory("BrassKnuckles_Dull");
            if (knux)
            {
                InventoryUtil.SetRange(knux, 0.45, 0.70);
                InventoryUtil.BindQuickbar(p, knux, 0);
            }

            EntityAI band = backpack.GetInventory().CreateInInventory("BandageDressing");
            if (band)
            {
                InventoryUtil.SetPristine(band);
                InventoryUtil.BindQuickbar(p, band, 1);
            }

            EntityAI heat = backpack.GetInventory().CreateInInventory("Heatpack");
            if (heat) InventoryUtil.SetRange(heat, 0.45, 0.70);
        }
    }
}
