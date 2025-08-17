// BallCapper, similar to base, based around wearing ballcap
class Theme_BallCapper
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        ref array<string> hat = {
            "BaseballCap_Beige","BaseballCap_Black","BaseballCap_Blue","BaseballCap_CMMG_Black",
            "BaseballCap_CMMG_Pink","BaseballCap_Camo","BaseballCap_Olive","BaseballCap_Pink","BaseballCap_Red"
        };
        string hatPick = hat[Math.RandomInt(0, hat.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Headgear", hatPick, 0.45, 0.75);

        ref array<string> shirts = {
            "TShirt_Beige","TShirt_Black","TShirt_Blue","TShirt_Green","TShirt_Grey",
            "TShirt_OrangeWhiteStripes","TShirt_Red","TShirt_RedBlackStripes","TShirt_White"
        };
        string shirtPick = shirts[Math.RandomInt(0, shirts.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Body", shirtPick, 0.45, 0.75);

        ref array<string> pants = {
            "Breeches_Beetcheck","Breeches_Beige","Breeches_Black","Breeches_Blackcheck",
            "Breeches_Blue","Breeches_Browncheck","Breeches_Green","Breeches_Pink","Breeches_Red","Breeches_White"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsPick, 0.45, 0.75);

        ref array<string> shoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown","AthleticShoes_Green","AthleticShoes_Grey",
            "Sneakers_Black","Sneakers_Gray","Sneakers_Green","Sneakers_Red","Sneakers_White"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.75, 1.0);

        if (Math.RandomFloat01() < 0.42) {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        for (int i = 0; i < 2; i++) {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }
    }
}
