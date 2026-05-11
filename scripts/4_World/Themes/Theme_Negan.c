// Negan with bat
class Theme_Negan
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        ref array<string> pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey" };
        ref array<string> shoeChoices  = { "CombatBoots_Grey","CombatBoots_Black" };

        // Equip
        InventoryUtil.ReplaceSlotServer(p, "Body",  "RidersJacket_Black", 0.55, 0.90);
        InventoryUtil.ReplaceSlotServer(p, "Legs",  pantsChoices[Math.RandomInt(0, pantsChoices.Count())], 0.45, 0.65);
        InventoryUtil.ReplaceSlotServer(p, "Feet",  shoeChoices[Math.RandomInt(0,  shoeChoices.Count())],   0.25, 0.4);

        // Items
        EntityAI bat = InventoryUtil.ReplaceSlotServer(p, "Melee", "BarbedBaseballBat", 1.0, 1.0);

        if (!bat) {
            bat = InventoryUtil.ReplaceSlotServer(p, "Shoulder", "BarbedBaseballBat", 1.0, 1.0);
        }

        if (!bat) {
            bat = InventoryUtil.PutInAnyClothesCargo(p, "BarbedBaseballBat");
        }

        if (bat) {
            InventoryUtil.SetWorn(bat);
            InventoryUtil.BindQuickbar(p, bat, 0);
        }
    }
}
