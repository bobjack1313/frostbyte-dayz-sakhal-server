// Professional: matched suit + slacks (sex-based), shoes worn, pants very damaged, top damaged→worn.
// Items: DisinfectantAlcohol (~2 uses), 2x SmallStone, random Pen. No knife.
// Glasses: 42% chance of Thick/Thin frames. Chemlight/food/armband are handled in your common step.
// Professional: matched suit + slacks (sex-based), shoes worn, pants very damaged, top damaged→worn.
// Items: DisinfectantAlcohol (~2 uses), 2x SmallStone, random Pen. No knife.
// Glasses: 42% chance of Thick/Thin frames. Chemlight/food/armband are handled in your common step.
class Theme_Professional
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        autoptr array<string> maleCols = { "White","LightGrey","Khaki","DarkGrey","Brown","Blue","Black","Beige" };
        autoptr array<string> femaleCols = { "White","LightGrey","Khaki","DarkGrey","Brown","Black","Beige" };

        int idx;
        if (male)
        {
            idx = Math.RandomInt(0, maleCols.Count());
        }
        else
        {
            idx = Math.RandomInt(0, femaleCols.Count());
        }

        string topCls;
        string pantsCls;

        if (male)
        {
            topCls   = "ManSuit_" + maleCols[idx];
            pantsCls = "SlacksPants_" + maleCols[idx];
        }
        else
        {
            topCls   = "WomanSuit_" + femaleCols[idx];
            pantsCls = "SlacksPants_" + femaleCols[idx];
        }

        // worn suit top, very damaged pants
        InventoryUtil.ReplaceSlotServer(p, "Body", topCls,   0.45, 0.70);
        InventoryUtil.ReplaceSlotServer(p, "Legs", pantsCls, 0.15, 0.35);

        // dress shoes
        autoptr array<string> shoes = { "DressShoes_Beige","DressShoes_Brown","DressShoes_Black","DressShoes_White","DressShoes_Sunburst" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        InventoryUtil.ReplaceSlotServer(p, "Feet", shoePick, 0.45, 0.70);

        // 42% glasses
        if (Math.RandomFloat01() < 0.42)
        {
            autoptr array<string> eyewear = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = eyewear[Math.RandomInt(0, eyewear.Count())];
            InventoryUtil.ReplaceSlotServer(p, "Eyewear", framePick, 0.65, 0.80);
        }

        // ~1-use alcohol on QB1
        ItemBase alc = ItemBase.Cast(InventoryUtil.PutInAnyClothesCargo(p, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); InventoryUtil.SetWorn(alc); InventoryUtil.BindQuickbar(p, alc, 1); }

        // two stones
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = InventoryUtil.PutInAnyClothesCargo(p, "SmallStone");
            if (stone) InventoryUtil.SetRandom(stone);
        }

        // pen (pristine)
        autoptr array<string> pens = { "Pen_Green","Pen_Blue","Pen_Black" };
        EntityAI pen = InventoryUtil.PutInAnyClothesCargo(p, pens[Math.RandomInt(0, pens.Count())]);
        if (pen) InventoryUtil.SetPristine(pen);
    }
}
