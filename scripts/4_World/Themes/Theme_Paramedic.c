// Paramedic: matched color set, all clothing very damaged (custom ranges), shoes very damaged.
// Items: DisinfectantAlcohol (~1 use), 2x SmallStone. NO bandage, NO knife.
// Glasses: 42% chance (Thick/Thin).
class Theme_Paramedic
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // pick a color set
        autoptr array<string> cols = { "Green","Crimson","Blue" };
        string col = cols[Math.RandomInt(0, cols.Count())];

        // jacket + pants (10–20% health)
        InventoryUtil.ReplaceSlotServer(p, "Body", "ParamedicJacket_" + col, 0.10, 0.20);
        InventoryUtil.ReplaceSlotServer(p, "Legs", "ParamedicPants_"  + col, 0.10, 0.20);

        // shoes (20–30% health)
        autoptr array<string> shoes = { "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown","AthleticShoes_Green","AthleticShoes_Grey" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.20, 0.30);

        // 42% chance to add glasses
        if (Math.RandomFloat01() < 0.42)
        {
            autoptr array<string> frames = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, frames.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        // Items: alcohol (~1 use) + two stones
        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc)
        {
            alc.SetQuantity(20);                 // ~1 disinfect action
            InventoryUtil.SetRandom(alc);
            InventoryUtil.BindQuickbar(p, alc, 1);
        }

        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }
    }
}
