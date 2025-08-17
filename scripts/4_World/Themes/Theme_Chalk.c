// Vanilla clothes; add bandage + (only if absent) a steak knife.
class Theme_Chalk
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Bandage (pristine) on QB1
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        // Knife only if not already in hands as SteakKnife
        EntityAI ih = p.GetItemInHands();
        bool alreadyKnife = (ih && ih.GetType() == "SteakKnife");
        if (!alreadyKnife)
        {
            EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
            if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
        }
    }
}

