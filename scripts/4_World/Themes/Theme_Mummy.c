// Mummy: full improvised wrap set (all PRISTINE).
// Items: 6x Rag (DISINFECTED, PRISTINE) + 6x Rag (not disinfected, worn) + 1x BoneKnife (damaged).
class Theme_Mummy
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Full improvised wrap
        InventoryUtil.ReplaceSlotServer(p, "Legs",   "LegsCover_Improvised",  1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Body",   "TorsoCover_Improvised", 1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Headgear","HeadCover_Improvised",  1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Mask",   "FaceCover_Improvised",  1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Feet",   "FeetCover_Improvised",  1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Gloves", "HandsCover_Improvised", 1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Eyewear","EyePatch_Improvised",   1.0, 1.0);

        // Clean rags (6), guaranteed placement
        ItemBase ragsClean = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "Rag"));
        if (!ragsClean) ragsClean = ItemBase.Cast(p.GetHumanInventory().CreateInHands("Rag"));
        if (ragsClean) {
            ragsClean.SetQuantity(6);
            ragsClean.SetHealth01("", "", 1.0);
            ragsClean.SetCleanness(1);
            InventoryUtil.BindQuickbar(p, ragsClean, 1);
        }

        // Dirty rags (6), fallback to hands if needed
        ItemBase ragsDirty = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "Rag"));
        if (!ragsDirty) ragsDirty = ItemBase.Cast(p.GetHumanInventory().CreateInHands("Rag"));
        if (ragsDirty) {
            ragsDirty.SetQuantity(6);
            ragsDirty.SetHealth01("", "", Math.RandomFloat(0.45, 0.70));
            ragsDirty.SetCleanness(0);
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
