class Theme_GrizzlyAdams
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Coat
        autoptr array<string> coats = { "WoolCoat_Beige","WoolCoat_Black","WoolCoat_Blue","WoolCoat_BrownCheck","WoolCoat_Green","WoolCoat_Red" };
        string coatPick = coats[Math.RandomInt(0, coats.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", coatPick, 0.25, 0.55);

        // Pants
        autoptr array<string> pants = { "HunterPants_Brown","HunterPants_Autumn","HunterPants_Spring","HunterPants_Summer" };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.25, 0.35);

        // shoes (random color)
        autoptr array<string> shoes = { "LeatherShoes_Beige","LeatherShoes_Black","LeatherShoes_Brown","LeatherShoes_Natural" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.50, 0.75);

        // bear headdress
        InventoryUtil.ReplaceSlotServer(p, "Headgear", "Headdress_Bear", 0.65, 0.80);

        // Clean rags (1-4), guaranteed placement
        ItemBase ragsClean = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "Rag"));
        if (!ragsClean) ragsClean = ItemBase.Cast(p.GetHumanInventory().CreateInHands("Rag"));
        if (ragsClean) {
            ragsClean.SetQuantity(Math.RandomInt(1,2));
            ragsClean.SetHealth01("", "", 1.0);
            ragsClean.SetCleanness(1);
            InventoryUtil.BindQuickbar(p, ragsClean, 1);
        }

        EntityAI trap = p.GetHumanInventory().CreateInHands("BearTrap");
        if (trap) {
            InventoryUtil.SetRandom(trap);
            InventoryUtil.BindQuickbar(p, trap, 0);
        }

        // Bone knife (damaged), with hands fallback
        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (!knife) knife = p.GetHumanInventory().CreateInHands("BoneKnife");
        if (knife) {
            InventoryUtil.SetLow(knife);
            InventoryUtil.BindQuickbar(p, knife, 0);
        }
    }
}
