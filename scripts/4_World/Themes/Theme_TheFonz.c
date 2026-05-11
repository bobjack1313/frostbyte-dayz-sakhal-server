// The Fonz biker look
class Theme_TheFonz
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        ref array<string> helmetChoices = { "MotoHelmet_Black","MotoHelmet_Blue","MotoHelmet_Red","MotoHelmet_Green","MotoHelmet_Chenarus" };
        ref array<string> pantsChoices = { "Jeans_BlueDark","Jeans_Grey","Jeans_Blue" };
        ref array<string> shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White","Sneakers_Green","Sneakers_Red" };

        // Equip
        InventoryUtil.ReplaceSlotServer(p, "Headgear", helmetChoices[Math.RandomInt(0, helmetChoices.Count())], 0.32, 0.55);
        InventoryUtil.ReplaceSlotServer(p, "Body",  "RidersJacket_Black", 0.35, 0.55);
        InventoryUtil.ReplaceSlotServer(p, "Legs",  pantsChoices[Math.RandomInt(0, pantsChoices.Count())], 0.45, 0.69);
        InventoryUtil.ReplaceSlotServer(p, "Feet",  shoeChoices[Math.RandomInt(0,  shoeChoices.Count())],   0.55, 0.75);
        InventoryUtil.ReplaceSlotServer(p, "Eyewear", "AviatorGlasses", 0.65, 0.80);

        // Items
        // Clean rags (2-5), guaranteed placement
        ItemBase ragsClean = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "Rag"));
        if (!ragsClean) ragsClean = ItemBase.Cast(p.GetHumanInventory().CreateInHands("Rag"));
        if (ragsClean) {
            ragsClean.SetQuantity(Math.RandomInt(2,5));
            ragsClean.SetHealth01("", "", 1.0);
            ragsClean.SetCleanness(1);
            InventoryUtil.BindQuickbar(p, ragsClean, 1);
        }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
