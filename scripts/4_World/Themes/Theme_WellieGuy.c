// Wellie Guy: pristine raincoat; matching boonie + skirt (worn); wellies random (worn).
// // Bandage given. NO ARMBAND (call GiveCommonItems(player, /*skipArmband=*/true) in controller).
class Theme_WellieGuy
{
    static void Apply(PlayerBase p, bool male = true)
    {
        if (!p) return;

        // Palette
        autoptr array<string> coats = {
            "Raincoat_Black","Raincoat_Blue","Raincoat_Green","Raincoat_Orange",
            "Raincoat_Red","Raincoat_Pink","Raincoat_Yellow"
        };
        autoptr array<string> wellies = { "Wellies_Grey","Wellies_Green","Wellies_Black","Wellies_Brown" };

        // pick coat
        string coat = coats[Math.RandomInt(0, coats.Count())];

        // map coat -> boonie candidates & skirt color
        ref array<string> hatChoices = new array<string>;
        string skirt = "DenimSkirt"; // safe fallback

        switch (coat)
        {
            case "Raincoat_Blue":
                hatChoices.Insert("BoonieHat_NavyBlue");
                skirt = "Skirt_Blue";
                break;

            case "Raincoat_Green":
                hatChoices.Insert("BoonieHat_Olive");
                skirt = "Skirt_White";
                break;

            case "Raincoat_Orange":
                hatChoices.Insert("BoonieHat_Orange");
                skirt = "Skirt_Yellow";
                break;

            case "Raincoat_Red":
                hatChoices.Insert("BoonieHat_Red");
                skirt = "Skirt_Red";
                break;

            case "Raincoat_Pink":
                hatChoices.Insert("BoonieHat_Black");
                hatChoices.Insert("BoonieHat_NavyBlue");
                skirt = "Skirt_White";
                break;

            case "Raincoat_Yellow":
                hatChoices.Insert("BoonieHat_Black");
                hatChoices.Insert("BoonieHat_Blue");
                hatChoices.Insert("BoonieHat_NavyBlue");
                hatChoices.Insert("BoonieHat_Olive");
                skirt = "Skirt_Yellow";
                break;

            case "Raincoat_Black":
            default:
                hatChoices.Insert("BoonieHat_Black");
                skirt = "Skirt_Blue";
                break;
        }

        // Equip: raincoat PRISTINE; others worn
        InventoryUtil.ReplaceSlotServer(p, "Body", coat, 1.0, 1.0);
        InventoryUtil.ReplaceSlotServer(p, "Legs", skirt, 0.52, 0.69);
        InventoryUtil.ReplaceSlotServer(p, "Headgear", hatChoices[Math.RandomInt(0, hatChoices.Count())], 0.53, 0.68);
        InventoryUtil.ReplaceSlotServer(p, "Feet", wellies[Math.RandomInt(0, wellies.Count())], 0.45, 0.69);

        // Try adding a hook to the hat
        EntityAI hat = p.FindAttachmentBySlotName("Headgear");
        if (hat && Math.RandomFloat01() < 0.66)
        {
            EntityAI hook = hat.GetInventory().CreateAttachment("Hook");
            if (!hook) hook = hat.GetInventory().CreateAttachment("FishingHook");
            if (!hook) hook = hat.GetInventory().CreateAttachment("BoneHook");
            if (hook) InventoryUtil.SetPristine(hook);
        }

        // Items: bandage (pristine) + bone knife (low)
        EntityAI band = InventoryUtil.PutInAnyClothesCargo(p, "BandageDressing");
        if (band) { InventoryUtil.SetPristine(band); InventoryUtil.BindQuickbar(p, band, 1); }

        EntityAI knife = InventoryUtil.PutInAnyClothesCargo(p, "BoneKnife");
        if (!knife) knife = p.GetHumanInventory().CreateInHands("BoneKnife");
        if (knife) { InventoryUtil.SetLow(knife); InventoryUtil.BindQuickbar(p, knife, 0); }
    }
}
