// Thug, similar to base, bandanna added
class Theme_Thug
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Palette
        autoptr array<string> bandanas = {
            "Bandana_BlackPattern","Bandana_Blue","Bandana_Greenpattern","Bandana_Pink",
            "Bandana_PolkaPattern","Bandana_RedPattern","Bandana_Yellow"
        };
        string bandana = bandanas[Math.RandomInt(0, bandanas.Count())];

        // Map bandana -> outfit pools
        ref array<string> pantsChoices = new array<string>;
        ref array<string> shirtChoices = new array<string>;
        ref array<string> shoeChoices  = new array<string>;

        switch (bandana)
        {
            case "Bandana_Blue":
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey","Jeans_Blue","Jeans_Green" };
                shirtChoices = { "TShirt_White","TShirt_Grey","TShirt_Black","TShirt_Beige" };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White" };
                break;

            case "Bandana_Greenpattern":
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey","Jeans_Brown" };
                shirtChoices = { "TShirt_White","TShirt_Grey","TShirt_Black" };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White","Sneakers_Green" };
                break;

            case "Bandana_Pink":
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey" };
                shirtChoices = { "TShirt_White","TShirt_Grey","TShirt_Black" };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White","Sneakers_Green" };
                break;

            case "Bandana_PolkaPattern":
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey","Jeans_Green" };
                shirtChoices = { "TShirt_White","TShirt_Grey","TShirt_Black","TShirt_Beige","TShirt_Green","TShirt_OrangeWhiteStripes" };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White" };
                break;

            case "Bandana_RedPattern":
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey","Jeans_Blue","Jeans_Brown" };
                shirtChoices = { "TShirt_White","TShirt_Grey","TShirt_Black","TShirt_Red","TShirt_Green","TShirt_RedBlackStripes" };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White","Sneakers_Red" };
                break;

            case "Bandana_Yellow":
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey","Jeans_Blue","Jeans_Brown","Jeans_Green" };
                shirtChoices = { "TShirt_White","TShirt_Grey","TShirt_Black","TShirt_Green","TShirt_Blue" };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White","Sneakers_Red" };
                break;

            case "Bandana_BlackPattern":
            default:
                pantsChoices = { "Jeans_BlueDark","Jeans_Black","Jeans_Grey","Jeans_Blue","Jeans_Brown","Jeans_Green" };
                shirtChoices = {
                    "TShirt_White","TShirt_Grey","TShirt_Black","TShirt_Green","TShirt_Blue",
                    "TShirt_OrangeWhiteStripes","TShirt_RedBlackStripes","TShirt_Red","TShirt_Beige"
                };
                shoeChoices  = { "Sneakers_Black","Sneakers_Gray","Sneakers_White","Sneakers_Red" };
                break;
        }

        // Equip
        InventoryUtil.ReplaceSlotServer(p, "Headgear",  bandana, 0.45, 0.75);
        InventoryUtil.ReplaceSlotServer(p, "Body",  shirtChoices[Math.RandomInt(0, shirtChoices.Count())], 0.52, 0.75);
        InventoryUtil.ReplaceSlotServer(p, "Legs",  pantsChoices[Math.RandomInt(0, pantsChoices.Count())], 0.45, 0.69);
        InventoryUtil.ReplaceSlotServer(p, "Feet",  shoeChoices[Math.RandomInt(0,  shoeChoices.Count())],   0.55, 0.75);
        InventoryUtil.ReplaceSlotServer(p, "Eyewear", "DesignerGlasses", 0.65, 0.80);

        // Items
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
