// Cowboy/Cowgirl: hat + shirt worn/damaged; jeans (male) or denim shorts (female) worn/damaged; boots very damaged.
// Standard items in Body: bandage (random health), steak knife (very damaged), chemlight (random health).
class Theme_Cowboy
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Pools
        autoptr array<string> cowboyHats = {
            "CowboyHat_green","CowboyHat_darkBrown","CowboyHat_black","CowboyHat_Brown"
        };
        autoptr array<string> cowboyShirts = {
	         "Shirt_BlueCheck","Shirt_BlueCheckBright","Shirt_GreenCheck","Shirt_PlaneBlack","Shirt_RedCheck","Shirt_WhiteCheck"
        };
        autoptr array<string> jeansMale = {
            "Jeans_Brown","Jeans_Black","Jeans_BlueDark","Jeans_Green","Jeans_Gray"
        };
        autoptr array<string> shortsFemale = {
            "ShortJeans_Black","ShortJeans_Red","ShortJeans_Green","ShortJeans_Darkblue","ShortJeans_Brown","ShortJeans_Blue"
        };
        autoptr array<string> cowboyBoots = {
            "WorkingBoots_Beige","WorkingBoots_Brown","WorkingBoots_Green","WorkingBoots_Grey","WorkingBoots_Yellow"
        };

        // Picks
        string hat   = cowboyHats[Math.RandomInt(0, cowboyHats.Count())];
        string shirt = cowboyShirts[Math.RandomInt(0, cowboyShirts.Count())];

        string legs;
        if (male)
            legs = jeansMale[Math.RandomInt(0, jeansMale.Count())];
        else
            legs = shortsFemale[Math.RandomInt(0, shortsFemale.Count())];

        string boots = cowboyBoots[Math.RandomInt(0, cowboyBoots.Count())];

        // NOTE: slot is "Head" (not "Headgear")
        InventoryUtil.ReplaceSlotServer(p, "Headgear", hat,   0.65, 0.80);
        InventoryUtil.ReplaceSlotServer(p, "Body", shirt, 0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Legs", legs,  0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Feet", boots, 0.25, 0.30);

        // Cargo items
        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "SteakKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }

        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }
    }
}
