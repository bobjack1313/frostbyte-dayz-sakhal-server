// Track Star: full tracksuit and running shoes
// Nerf on clothes, Two stones and alcohol tincture
class Theme_TrackStar
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Color pick
        autoptr array<string> cols = { "Blue","Green","Red","LightBlue","Black" };
        string col = cols[Math.RandomInt(0, cols.Count())];

        // Outfit
        InventoryUtil.ReplaceSlotServer(p, "Body", "TrackSuitJacket_" + col, 0.15, 0.40); // heavier nerf
        InventoryUtil.ReplaceSlotServer(p, "Legs", "TrackSuitPants_"  + col, 0.45, 0.60); // lighter nerf

        // Shoes
        autoptr array<string> shoes = { "JoggingShoes_Black","JoggingShoes_Blue","JoggingShoes_Red","JoggingShoes_Violet","JoggingShoes_White" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.60);

        // 66% sport glasses
        if (Math.RandomFloat01() < 0.66)
        {
            autoptr array<string> frames = { "SportGlasses_Black","SportGlasses_Blue","SportGlasses_Green","SportGlasses_Orange" };
            string framePick = frames[Math.RandomInt(0, frames.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        // Items
        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); InventoryUtil.SetRandom(alc); InventoryUtil.BindQuickbar(p, alc, 1); }

        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }
    }
}
