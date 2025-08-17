// Sailor, same as base (BoneKnife variant)
class Theme_Sailor
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        InventoryUtil.ReplaceSlotServer(p, "Body", "TelnyashkaShirt", 0.45, 0.75);
        if (p.FindAttachmentBySlotName("Body") && p.FindAttachmentBySlotName("Body").GetType() != "TelnyashkaShirt") {
            ACHLog("[ACH][Sailor] WARN Body not Telnyashka after replace");
        }

        // NOTE: some servers use NavalUniformPants; your current set uses NavyUniformPants
        InventoryUtil.ReplaceSlotServer(p, "Legs", "NavyUniformPants", 0.45, 0.75);
        if (p.FindAttachmentBySlotName("Legs") && p.FindAttachmentBySlotName("Legs").GetType() != "NavyUniformPants") {
            ACHLog("[ACH][Sailor] WARN Legs not Naval/NavyUniformPants after replace");
        }

        autoptr array<string> shoes = { "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Grey" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.75);

        // Damaged Bone Knife (no bandage)
        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }

        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }
    }
}
