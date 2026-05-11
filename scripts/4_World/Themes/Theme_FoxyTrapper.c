class Theme_FoxyTrapper
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Coat
        autoptr array<string> coats = { "Sweater_Blue","Sweater_Green","Sweater_Gray" };
        string coatPick = coats[Math.RandomInt(0, coats.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", coatPick, 0.25, 0.55);

        // Pants
        autoptr array<string> pants = { "HunterPants_Brown","HunterPants_Autumn","HunterPants_Spring","HunterPants_Summer" };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.25, 0.35);

        // shoes (random color)
        autoptr array<string> trapperShoes = { "LeatherShoes_Beige","LeatherShoes_Black","LeatherShoes_Brown","LeatherShoes_Natural" };
        string shoePick = trapperShoes[Math.RandomInt(0, trapperShoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.50, 0.75);

        // Fox headdress
        InventoryUtil.ReplaceSlotServer(p, "Headgear", "Headdress_Fox", 0.65, 0.80);

        // Damaged Bone Knife (no bandage)
        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }

        // Rabbit snares (3 total)
        for (int i = 0; i < 2; i++)
        {
            EntityAI cargoTrap = InventoryUtil.PutInAnyClothesCargo(p, "RabbitSnareTrap");
            if (cargoTrap) {
                InventoryUtil.SetPristine(cargoTrap);
            }
        }

        // One in hands
        EntityAI handTrap = p.GetHumanInventory().CreateInHands("RabbitSnareTrap");
        if (handTrap) {
            InventoryUtil.SetPristine(handTrap);
            InventoryUtil.BindQuickbar(p, handTrap, 1);
        }
    }
}
