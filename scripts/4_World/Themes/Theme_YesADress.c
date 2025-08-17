// Yes... a Dress
class Theme_YesADress
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Dress
        autoptr array<string> dress = {
            "MiniDress_BlueChecker","MiniDress_BlueWithDots","MiniDress_BrownChecker","MiniDress_GreenChecker",
            "MiniDress_Pink","MiniDress_PinkChecker","MiniDress_RedChecker","MiniDress_WhiteChecker"
        };
        string dressPick = dress[Math.RandomInt(0, dress.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", dressPick, 0.75, 1.0);

        // Shoes
        autoptr array<string> shoes = { "Sneakers_Black","Sneakers_Gray","Sneakers_Green","Sneakers_Red","Sneakers_White" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.75, 1.0);

        // No pants with the dress
        InventoryUtil.ClearSlotServer(p, "Legs");

        // Kid backpack
        autoptr array<string> kidBags = { "ChildBag_Blue","ChildBag_Green","ChildBag_Red" };
        string bagPick = kidBags[Math.RandomInt(0, kidBags.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Back", bagPick, 0.75, 1.0);

        // Bandage (QB1) + SteakKnife (QB0)
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }

        // Bear friend
        autoptr array<string> bears = { "Bear_Beige","Bear_White","Bear_Dark","Bear_Pink" };
        string bearPick = bears[Math.RandomInt(0, bears.Count())];
        EntityAI bearMe = InventoryUtil.PutInAnyClothesCargo(p, bearPick);
        if (bearMe) InventoryUtil.SetPristine(bearMe);
    }
}
