// FlatCapper, similar to base, based around wearing flatcap
class Theme_FlatCapper
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Hat
        autoptr array<string> hats = {
            "FlatCap_Black","FlatCap_BlackCheck","FlatCap_Blue","FlatCap_Brown","FlatCap_BrownCheck","FlatCap_Grey",
            "FlatCap_GreyCheck","FlatCap_Red"
        };
        string hatPick = hats[Math.RandomInt(0, hats.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Headgear", hatPick, 0.45, 0.75);

        // Shirt (male vs female)
        if (male)
        {
            autoptr array<string> shirts = {
                "TShirt_Beige","TShirt_Black","TShirt_Blue","TShirt_Green","TShirt_Grey",
                "TShirt_OrangeWhiteStripes","TShirt_Red","TShirt_RedBlackStripes","TShirt_White"
            };
            string shirtPick = shirts[Math.RandomInt(0, shirts.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Body", shirtPick, 0.45, 0.75);
        }
        else
        {
            autoptr array<string> femShirts = { "Blouse_Blue","Blouse_Green","Blouse_Violet","Blouse_White" };
            string femShirtPick = femShirts[Math.RandomInt(0, femShirts.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Body", femShirtPick, 0.45, 0.75);
        }

        // Pants
        autoptr array<string> pants = {
            "Breeches_Beetcheck","Breeches_Beige","Breeches_Black","Breeches_Blackcheck","Breeches_Blue",
            "Breeches_Browncheck","Breeches_Green","Breeches_Pink","Breeches_Red","Breeches_White"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.45, 0.75);

        // Shoes
        autoptr array<string> shoes = { "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown","AthleticShoes_Green","AthleticShoes_Grey" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.75, 1.0);

        // 42% chance to wear glasses
        if (Math.RandomFloat01() < 0.42)
        {
            autoptr array<string> frames = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, frames.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        // Items
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }
    }
}
