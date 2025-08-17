class Theme_Cop
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        InventoryUtil.ReplaceSlotServer(p, "Body",    "PoliceJacket",   0.15, 0.30);
        InventoryUtil.ReplaceSlotServer(p, "Legs",    "PolicePants",    0.15, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Eyewear", "AviatorGlasses", 0.45, 0.70);

        ref array<string> copShoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown",
            "AthleticShoes_Green","AthleticShoes_Grey"
        };
        string shoePick = copShoes[Math.RandomInt(0, copShoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) {
            alc.SetQuantity(20);
            InventoryUtil.SetWorn(alc);
            InventoryUtil.BindQuickbar(p, alc, 1);
        }

        EntityAI keys = InventoryUtil.PutInAnyClothesCargo(p, "HandcuffKeys");
        if (keys) InventoryUtil.SetWorn(keys);

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
        if (knife) {
            InventoryUtil.SetLow(knife);
            InventoryUtil.BindQuickbar(p, knife, 0);
        }
    }
}
