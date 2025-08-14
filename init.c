void main()
{
    // INIT ECONOMY --------------------------------------
    Hive ce = CreateHive();
    if (ce) ce.InitOffline();

    // DATE RESET AFTER ECONOMY INIT ---------------------
    int year, month, day, hour, minute;
    int reset_month = 2, reset_day = 1;
    GetGame().GetWorld().GetDate(year, month, day, hour, minute);

    if ((month == reset_month) && (day < reset_day))
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    else if ((month == reset_month + 1) && (day > reset_day))
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    else if ((month < reset_month) || (month > reset_month + 1))
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
}

class CustomMission: MissionServer
{
    int mThemeIndex; // advances each spawn

    void CustomMission()
    {
        mThemeIndex = LoadThemeIndex(); // optional persistence
    }

    // Persist across restarts (in $profile)
    int LoadThemeIndex()
    {
        FileHandle f = OpenFile("$profile:ach_theme_index.txt", FileMode.READ);
        if (f) {
            string s;
            FGets(f, s);
            CloseFile(f);
            return s.ToInt();
        }
        return 0;
    }


    void SaveThemeIndex()
    {
        FileHandle f = OpenFile("$profile:ach_theme_index.txt", FileMode.WRITE);
        if (f) {
            FPrintln(f, mThemeIndex.ToString());
            CloseFile(f);
        }
    }


    // ---------- Base helpers ----------
    void SetRandomHealth(EntityAI itemEnt)
    {
        if (itemEnt) itemEnt.SetHealth01("", "", Math.RandomFloat(0.31, 0.80));
    }


    void SetLowHealth(EntityAI itemEnt)
    {
        if (itemEnt) itemEnt.SetHealth01("", "", Math.RandomFloat(0.20, 0.39));
    }


    void SetWorn(EntityAI itemEnt)
    {
        if (itemEnt) itemEnt.SetHealth01("", "", Math.RandomFloat(0.50, 0.69));
    }


    void SetPristine(EntityAI itemEnt)
    {
        if (itemEnt) itemEnt.SetHealth01("", "", 1.0);
    }

    void ACHLog(string s)
    {
        FileHandle fh = OpenFile("$profile:ach.log", FileMode.APPEND);
        if (fh != 0)
        {
            string t; GetGame().GetWorld().GetDate(NULL, NULL, NULL, NULL, NULL);
            FPrintln(fh, s); // keep it simple
            CloseFile(fh);
        }
    }

    void ClearSlotServer(PlayerBase p, string slotName)
    {
        EntityAI e = p.FindAttachmentBySlotName(slotName);
        if (e) GetGame().ObjectDelete(e);
    }

    void DumpWear(PlayerBase p, string tag)
    {
        TStringArray slots = new TStringArray;
        slots.Insert("Headgear");
        slots.Insert("Body");
        slots.Insert("Legs");
        slots.Insert("Feet");

        string msg = "[ACH][DUMP][" + tag + "] ";

        for (int i = 0; i < slots.Count(); i++) {
            string s = slots[i];
            EntityAI a = p.FindAttachmentBySlotName(s);
            string val = "none";
            if (a) {
                val = a.GetType();
            }
            msg = msg + s + "=" + val + "  ";
        }

        ACHLog(msg);
    }


    EntityAI PutInAnyClothingCargo(PlayerBase p, string itemClass)
    {
        array<string> slots = {"Body","Legs","Feet","Headgear","Vest","Back"};
        for (int i = 0; i < slots.Count(); i++) {
            EntityAI clothing = p.FindAttachmentBySlotName(slots[i]);
            if (clothing) {
                EntityAI item = clothing.GetInventory().CreateInInventory(itemClass);
                if (item) {
                    //ACHLog("[ACH][Cargo] placed " + itemClass + " in " + slots[i]);
                    return item;
                }
            }
        }
        ACHLog("[ACH][Cargo] failed to place " + itemClass);
        return null;
    }


    EntityAI ReplaceSlotServer(PlayerBase p, string slotName, string cls, float hMin, float hMax)
    {
        ACHLog("[ACH][ReplaceSV] slot=" + slotName + " cls=" + cls);

        // 1) Kill whatever's there (server-authoritative)
        EntityAI existing = p.FindAttachmentBySlotName(slotName);
        if (existing) {
            //ACHLog("[ACH][ReplaceSV] deleting existing " + existing.GetType());
            GetGame().ObjectDelete(existing);
        }

        int slotId = InventorySlots.GetSlotIdFromString(slotName);

        // 2) Try direct attach first
        EntityAI a = p.GetInventory().CreateAttachmentEx(cls, slotId);
        if (!a) {
            a = p.GetInventory().CreateAttachment(cls);
        }

        // 3) If still not there, create in inventory and server-take into slot
        if (!p.FindAttachmentBySlotName(slotName)) {
            EntityAI inv = p.GetInventory().CreateInInventory(cls);
            if (inv) {
                p.GetInventory().TakeEntityAsAttachmentEx(InventoryMode.SERVER, inv, slotId);
            } else {
                ACHLog("[ACH][ReplaceSV] CreateInInventory failed for " + cls);
            }
        }

        // 4) Verify final state only
        EntityAI check = p.FindAttachmentBySlotName(slotName);
        if (check && check.GetType() == cls) {
            check.SetHealth01("", "", Math.RandomFloat(hMin, hMax));
            //ACHLog("[ACH][ReplaceSV] OK " + slotName + "=" + check.GetType());
            return check;
        }

        //ACHLog("[ACH][ReplaceSV] VERIFY FAIL slot=" + slotName + " cls=" + cls);
        return null;
    }


    // Simple armband for now (swap later for your balanced system)
    void GiveRandomArmband(PlayerBase player)
    {
        ref array<string> colors = { "Black","Blue","Green","Orange","Red","White","Yellow","Pink" };
        string cls = "Armband_" + colors[Math.RandomInt(0, colors.Count())];

        // remove any existing armband (server-authoritative)
        EntityAI existing = player.FindAttachmentBySlotName("Armband");
        if (existing) {
            GetGame().ObjectDelete(existing);
        }

        int slotId = InventorySlots.GetSlotIdFromString("Armband");

        // try direct slot attach first
        EntityAI a = player.GetInventory().CreateAttachmentEx(cls, slotId);
        if (!a) {
            // fallback: create in inventory, then server-take into slot
            EntityAI inv = player.GetInventory().CreateInInventory(cls);
            if (inv) {
                player.GetInventory().TakeEntityAsAttachmentEx(InventoryMode.SERVER, inv, slotId);
                a = player.FindAttachmentBySlotName("Armband");
            }
        }

        if (a) {
            SetPristine(a);
        } else {
            ACHLog("[ACH][Armband] attach failed for " + cls);
        }
    }


    // ---------- Themes (each self-contained) ----------

    // Wellie Guy: pristine raincoat; matching boonie + skirt (worn); wellies random (worn).
    // // Bandage given. NO ARMBAND (call GiveCommonItems(player, /*skipArmband=*/true) in controller).
    void EquipTheme_WellieGuy(PlayerBase player)
    {
        // Palette
        ref array<string> coats = {
            "Raincoat_Black","Raincoat_Blue","Raincoat_Green","Raincoat_Orange",
            "Raincoat_Red","Raincoat_Pink","Raincoat_Yellow"
        };
        ref array<string> wellies = { "Wellies_Grey","Wellies_Green","Wellies_Black","Wellies_Brown" };

        // pick coat
        string coat = coats[Math.RandomInt(0, coats.Count())];

        // map coat -> boonie candidates & skirt color (closest available)
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
        ReplaceSlotServer(player, "Body", coat, 1.0, 1.0);
        ReplaceSlotServer(player, "Legs", skirt, 0.52, 0.69);
        ReplaceSlotServer(player, "Headgear", hatChoices[Math.RandomInt(0, hatChoices.Count())], 0.53, 0.68);
        ReplaceSlotServer(player, "Feet", wellies[Math.RandomInt(0, wellies.Count())], 0.45, 0.69);


        EntityAI hat = player.FindAttachmentBySlotName("Headgear");
        if (hat && Math.RandomFloat01() < 0.66)
        {
            EntityAI hook = hat.GetInventory().CreateAttachment("Hook");
            if (!hook) hook = hat.GetInventory().CreateAttachment("FishingHook");
            if (!hook) hook = hat.GetInventory().CreateAttachment("BoneHook");
            if (hook) SetPristine(hook);
        }

        // Items: bandage (worn-ish)
        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        EntityAI knife = PutInAnyClothingCargo(player, "BoneKnife");
        if (!knife) knife = player.GetHumanInventory().CreateInHands("BoneKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    void EquipTheme_Shaman(PlayerBase player)
    {
        // hard-strip torso/legs
        EntityAI e = player.FindAttachmentBySlotName("Body"); if (e) e.Delete();
        e = player.FindAttachmentBySlotName("Legs"); if (e) e.Delete();

        // shoes (random color)
        ref array<string> shamanShoes = { "LeatherShoes_Beige","LeatherShoes_Black","LeatherShoes_Brown","LeatherShoes_Natural" };
        ReplaceSlotServer(player, "Feet", shamanShoes[Math.RandomInt(0, shamanShoes.Count())], 0.5, 0.75);

        // wolf headdress
        ReplaceSlotServer(player, "Head", "Headdress_Wolf", 0.65, 0.80);

        // bag — pristine (20 slots)
        ref array<string> shamanBag = { "SlingBag_Black", "SlingBag_Brown", "SlingBag_Gray" };

        EntityAI bag = ReplaceSlotServer(player, "Back", shamanBag[Math.RandomInt(0, shamanBag.Count())], 0.65, 0.90);

        // contents (in bag if possible; fallback to clothes if not)
        EntityAI band = null;
        EntityAI knife = null;

        if (bag)
        {
            band = bag.GetInventory().CreateInInventory("BandageDressing");
            knife = bag.GetInventory().CreateInInventory("BoneKnife");
        }
        if (!band)  band  = PutInAnyClothingCargo(player, "BandageDressing");
        if (!knife) knife = PutInAnyClothingCargo(player, "BoneKnife");

        if (band)  { band.SetHealth01("", "", 1.0);  player.SetQuickBarEntityShortcut(band, 1); }
        if (knife) { knife.SetHealth01("", "", 0.65); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    // Cowboy/Cowgirl: hat + shirt worn/damaged; jeans (male) or denim shorts (female) worn/damaged; boots very damaged.
    // Standard items in Body: bandage (random health), steak knife (very damaged), chemlight (random health).
    void EquipTheme_Cowboy(PlayerBase player, bool male)
    {
        // Pools
        ref array<string> cowboyHats = { "CowboyHat_green","CowboyHat_darkBrown","CowboyHat_black","CowboyHat_Brown" };
        ref array<string> cowboyShirts = {
            "Shirt_BlueCheck","Shirt_BlueCheckBright","Shirt_GreenCheck","Shirt_PlainBlack","Shirt_RedCheck","Shirt_WhiteCheck"
        };
        ref array<string> jeansMale = { "Jeans_Brown","Jeans_Black","Jeans_BlueDark","Jeans_Green","Jeans_Gray" };
        ref array<string> shortsFemale = {
            "ShortJeans_Black","ShortJeans_Red","ShortJeans_Green","ShortJeans_Darkblue","ShortJeans_Brown","ShortJeans_Blue"
        };
        ref array<string> cowboyBoots = { "WorkingBoots_Beige","WorkingBoots_Brown","WorkingBoots_Green","WorkingBoots_Grey","WorkingBoots_Yellow" };

        // Picks
        string hat   = cowboyHats[Math.RandomInt(0, cowboyHats.Count())];
        string shirt = cowboyShirts[Math.RandomInt(0, cowboyShirts.Count())];
        string legs;
        if (male) legs = jeansMale[Math.RandomInt(0, jeansMale.Count())];
        else      legs = shortsFemale[Math.RandomInt(0, shortsFemale.Count())];
        string boots = cowboyBoots[Math.RandomInt(0, cowboyBoots.Count())];

        // Server-authoritative replaces (note Headgear, not Head)
        ReplaceSlotServer(player, "Headgear", hat,   0.65, 0.80);
        ReplaceSlotServer(player, "Body",     shirt, 0.45, 0.70);
        ReplaceSlotServer(player, "Legs",     legs,  0.45, 0.70);
        ReplaceSlotServer(player, "Feet",     boots, 0.25, 0.30); // nerfed durability

        // Cargo items (server-safe)
        EntityAI knife = PutInAnyClothingCargo(player, "SteakKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }

        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }
    }


    // Prisoner: jacket + pants + cap (very damaged → worn), shoes worn (random color),
    // Bandage + 2x SmallStone + HandcuffsLocked. No knife.
    void EquipTheme_Prisoner(PlayerBase player)
    {
        ReplaceSlotServer(player, "Body",     "PrisonUniformJacket", 0.25, 0.75);
        ReplaceSlotServer(player, "Legs",     "PrisonUniformPants",  0.25, 0.75);
        ReplaceSlotServer(player, "Headgear", "PrisonerCap",         0.25, 0.75);

        ref array<string> prisonShoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown",
            "AthleticShoes_Green","AthleticShoes_Grey"
        };
        string shoePick = prisonShoes[Math.RandomInt(0, prisonShoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.70);

        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        EntityAI cuffs = PutInAnyClothingCargo(player, "HandcuffsLocked");
        if (cuffs) SetRandomHealth(cuffs);

        for (int i = 0; i < 2; i++) {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }
    }


    // Cop: jacket + pants (very damaged → worn), shoes worn (random color),
    // Alcohol (~2 uses) + steak knife + HandcuffKeys. No chemlight/food/armband here.
    void EquipTheme_Cop(PlayerBase player)
    {
        ReplaceSlotServer(player, "Body",    "PoliceJacket",   0.15, 0.30);
        ReplaceSlotServer(player, "Legs",    "PolicePants",    0.15, 0.70);
        ReplaceSlotServer(player, "Eyewear", "AviatorGlasses", 0.45, 0.70);

        ref array<string> copShoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown",
            "AthleticShoes_Green","AthleticShoes_Grey"
        };
        string shoePick = copShoes[Math.RandomInt(0, copShoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.70);

        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); SetWorn(alc); player.SetQuickBarEntityShortcut(alc, 1); }

        EntityAI keys = PutInAnyClothingCargo(player, "HandcuffKeys");
        if (keys) SetWorn(keys);

        EntityAI knife = PutInAnyClothingCargo(player, "SteakKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    // Professional: matched suit + slacks (sex-based), shoes worn, pants very damaged, top damaged→worn.
    // Items: DisinfectantAlcohol (~2 uses), 2x SmallStone, random Pen. No knife.
    // Glasses: 42% chance of Thick/Thin frames. Chemlight/food/armband are handled in your common step.
    void EquipTheme_Professional(PlayerBase player, bool male)
    {
        ref array<string> maleCols   = { "White","LightGrey","Khaki","DarkGrey","Brown","Blue","Black","Beige" };
        ref array<string> femaleCols = { "White","LightGrey","Khaki","DarkGrey","Brown","Black","Beige" };

        int idx;
        if (male)
            idx = Math.RandomInt(0, maleCols.Count());
        else
            idx = Math.RandomInt(0, femaleCols.Count());

        string topCls, pantsCls;
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

        ReplaceSlotServer(player, "Body", topCls,   0.45, 0.70); // worn
        ReplaceSlotServer(player, "Legs", pantsCls, 0.15, 0.35); // very damaged

        ref array<string> shoes = { "DressShoes_Beige","DressShoes_Brown","DressShoes_Black", "DressShoes_White","DressShoes_Sunburst" };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.70);

        if (Math.RandomFloat01() < 0.42) {
            string eyewear[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = eyewear[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); SetWorn(alc); player.SetQuickBarEntityShortcut(alc, 1); }

        for (int i = 0; i < 2; i++) {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }

        string pens[] = { "Pen_Green","Pen_Blue","Pen_Black" };
        EntityAI pen = PutInAnyClothingCargo(player, pens[Math.RandomInt(0, 3)]);
        if (pen) SetPristine(pen);
    }


    // Sorry Not Sorry (Naked Guy): no clothes, worn sneakers (random), worn 20-slot sling bag (random).
    // Bag contains: worn SteakKnife, worn BrassKnuckles_Dull, worn Heatpack.
    // Chemlight + UnknownFoodCan + armband are handled by your common step.
    void EquipTheme_SorryNotSorry(PlayerBase player)
    {
        // Always run after super.StartingEquipSetup on the SERVER.

        // 1) Hard strip (server-authoritative)
        ref array<string> stripSlots = { "Body","Legs","Headgear","Back" };
        for (int i = 0; i < stripSlots.Count(); i++) {
            EntityAI ex = player.FindAttachmentBySlotName(stripSlots[i]);
            if (ex) {
                GetGame().ObjectDelete(ex);
            }
        }

        // 2) Picks (random but deterministic per call)
        ref array<string> nakedShoes = { "Sneakers_Black","Sneakers_Gray","Sneakers_Green","Sneakers_Red","Sneakers_White" };
        ref array<string> slingBags  = { "SlingBag_Black","SlingBag_Brown","SlingBag_Gray" };

        string shoes = nakedShoes[Math.RandomInt(0, nakedShoes.Count())];
        string bag   = slingBags[Math.RandomInt(0, slingBags.Count())];

        // 3) Wearables (server-authoritative replaces)
        ReplaceSlotServer(player, "Feet",     shoes, 0.45, 0.70);
        ReplaceSlotServer(player, "Back",     bag,   0.45, 0.70);
        // Leave Body/Legs/Headgear intentionally empty per theme

        // 4) Put kit into bag (cargo), then quickbar
        EntityAI backpack = player.FindAttachmentBySlotName("Back");
        if (backpack) {
            EntityAI knife = backpack.GetInventory().CreateInInventory("SteakKnife");
            if (knife) {
                knife.SetHealth01("", "", Math.RandomFloat(0.45, 0.70));
            }

            EntityAI knux = backpack.GetInventory().CreateInInventory("BrassKnuckles_Dull");
            if (knux) {
                knux.SetHealth01("", "", Math.RandomFloat(0.45, 0.70));
                player.SetQuickBarEntityShortcut(knux, 0);
            }

            EntityAI band = backpack.GetInventory().CreateInInventory("BandageDressing");
            if (band) {
                band.SetHealth01("", "", Math.RandomFloat(1.0, 1.0));
                player.SetQuickBarEntityShortcut(band, 1);
            }

            EntityAI heat = backpack.GetInventory().CreateInInventory("Heatpack");
            if (heat) {
                heat.SetHealth01("", "", Math.RandomFloat(0.45, 0.70));
            }
        }
    }


    // Paramedic: matched color set, all clothing very damaged (custom ranges), shoes very damaged.
    // Items: DisinfectantAlcohol (~1 use), 2x SmallStone. NO bandage, NO knife.
    // Glasses: 42% chance (Thick/Thin).
    void EquipTheme_Paramedic(PlayerBase player)
    {
        // pick a color set
        ref array<string> cols = { "Green","Crimson","Blue" };
        string col = cols[Math.RandomInt(0, cols.Count())];

        // jacket + pants (10–20% health)
        ReplaceSlotServer(player, "Body", "ParamedicJacket_" + col, 0.10, 0.20);
        ReplaceSlotServer(player, "Legs", "ParamedicPants_" + col,   0.10, 0.20);

        // shoes (20–30% health)
        ref array<string> shoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Brown",
            "AthleticShoes_Green","AthleticShoes_Grey"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.20, 0.30);

        // 42% chance to add glasses
        if (Math.RandomFloat01() < 0.42)
        {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        // Items: alcohol (~1 use) + two stones
        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc)
        {
            alc.SetQuantity(10);           // ~1 disinfect action in vanilla
            SetRandomHealth(alc);
            player.SetQuickBarEntityShortcut(alc, 1);
        }

        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }
    }


    // Nurse: females = NurseDress_(White/Blue) + HipPack_Medical; males = matching scrubs set.
    // Shoes: random sneakers (worn). Glasses: 42% chance (Thick/Thin).
    // Items: DisinfectantAlcohol (~2 uses) + BoneKnife (damaged). No bandage.
    // NOTE: Common armband/chemlight/UnknownFoodCan come from GiveCommonItems(..).
    void EquipTheme_Nurse(PlayerBase player, bool male)
    {
        if (male)
        {
            ref array<string> cols = { "Blue","Green","White" };
            string col = cols[Math.RandomInt(0, cols.Count())];

            ReplaceSlotServer(player, "Body",     "MedicalScrubsShirt_" + col, 0.35, 0.60);
            ReplaceSlotServer(player, "Legs",     "MedicalScrubsPants_" + col, 0.35, 0.60);
            ReplaceSlotServer(player, "Headgear", "MedicalScrubsHat_"   + col, 0.35, 0.60);
        }
        else
        {
            ref array<string> dcols = { "White","Blue" };
            string dcol = dcols[Math.RandomInt(0, dcols.Count())];

            // make sure no pants survive under the dress
            ClearSlotServer(player, "Legs");

            ReplaceSlotServer(player, "Body", "NurseDress_" + dcol, 0.65, 0.75);

            // optional: clear Headgear if you want no hat with the dress
            // ClearSlotServer(player, "Headgear");

            ReplaceSlotServer(player, "Hips", "HipPack_Medical", 0.45, 0.70);
        }

        ref array<string> nurseShoes = {
            "Sneakers_Black","Sneakers_Gray","Sneakers_Green","Sneakers_Red","Sneakers_White"
        };
        string shoePick = nurseShoes[Math.RandomInt(0, nurseShoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.70);

        if (Math.RandomFloat01() < 0.42)
        {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc)
        {
            alc.SetQuantity(20);
            SetWorn(alc);
            player.SetQuickBarEntityShortcut(alc, 1);
        }

        EntityAI knife = PutInAnyClothingCargo(player, "BoneKnife");
        if (knife)
        {
            SetLowHealth(knife);
            player.SetQuickBarEntityShortcut(knife, 0);
        }
    }


    // Doctor: matching scrubs set (Blue/Green/White) + matching surgical gloves,
    // random dress shoes. Items: DisinfectantAlcohol (~2 uses) + BoneKnife (damaged).
    // No bandage. Common items come from GiveCommonItems(...).
    void EquipTheme_Doctor(PlayerBase player)
    {
        // pick a scrubs color and match all pieces
        ref array<string> cols = { "Blue","Green","White" };
        string col = cols[Math.RandomInt(0, cols.Count())];

        // scrubs + gloves + mask (worn-ish)
        ReplaceSlotServer(player, "Headgear", "MedicalScrubsHat_"   + col, 0.45, 0.70);
        ReplaceSlotServer(player, "Body",     "MedicalScrubsShirt_" + col, 0.45, 0.70);
        ReplaceSlotServer(player, "Legs",     "MedicalScrubsPants_" + col, 0.45, 0.70);
        ReplaceSlotServer(player, "Gloves",   "SurgicalGloves_"     + col, 0.45, 0.70);
        ReplaceSlotServer(player, "Mask",     "SurgicalMask",             0.45, 0.70);

        // random dress shoes (worn) — trim to known-good if you’re not using a mod
        ref array<string> shoes = {
            "DressShoes_Beige","DressShoes_Black","DressShoes_Brown" // add Sunburst/White only if your mod provides them
            // ,"DressShoes_Sunburst","DressShoes_White"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.70);

        // 64% chance to wear glasses
        if (Math.RandomFloat01() < 0.64) {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        // items: ~2-use alcohol + damaged bone knife (no bandage)
        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); SetRandomHealth(alc); player.SetQuickBarEntityShortcut(alc, 1); }

        EntityAI knife = PutInAnyClothingCargo(player, "BoneKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    // Mummy: full improvised wrap set (all PRISTINE).
    // Items: 6x Rag (DISINFECTED, PRISTINE) + 6x Rag (not disinfected, worn) + 1x BoneKnife (damaged).
    void EquipTheme_Mummy(PlayerBase player)
    {
        ReplaceSlotServer(player, "Legs",    "LegsCover_Improvised",   1.0, 1.0);
        ReplaceSlotServer(player, "Body",    "TorsoCover_Improvised",  1.0, 1.0);
        ReplaceSlotServer(player, "Headgear","HeadCover_Improvised",   1.0, 1.0);
        ReplaceSlotServer(player, "Mask",    "FaceCover_Improvised",   1.0, 1.0);
        ReplaceSlotServer(player, "Feet",    "FeetCover_Improvised",   1.0, 1.0);
        ReplaceSlotServer(player, "Gloves",  "HandsCover_Improvised",  1.0, 1.0);
        ReplaceSlotServer(player, "Eyewear", "EyePatch_Improvised",    1.0, 1.0);

        // Clean rags (6), guaranteed placement
        ItemBase ragsClean = ItemBase.Cast(PutInAnyClothingCargo(player, "Rag"));
        if (!ragsClean) ragsClean = ItemBase.Cast(player.GetHumanInventory().CreateInHands("Rag"));
        if (ragsClean) {
            ragsClean.SetQuantity(6);
            ragsClean.SetHealth01("", "", 1.0);
            ragsClean.SetCleanness(1);
            player.SetQuickBarEntityShortcut(ragsClean, 1);
        }

        // Dirty rags (6), mirror fallback so it always spawns
        ItemBase ragsDirty = ItemBase.Cast(PutInAnyClothingCargo(player, "Rag"));
        if (!ragsDirty) ragsDirty = ItemBase.Cast(player.GetHumanInventory().CreateInHands("Rag"));
        if (ragsDirty) {
            ragsDirty.SetQuantity(6);
            ragsDirty.SetHealth01("", "", Math.RandomFloat(0.45, 0.70));
            ragsDirty.SetCleanness(0);
        }

        // Bone knife (damaged), with hands fallback
        EntityAI knife = PutInAnyClothingCargo(player, "BoneKnife");
        if (!knife) knife = player.GetHumanInventory().CreateInHands("BoneKnife");
        if (knife) {
            SetLowHealth(knife);
            player.SetQuickBarEntityShortcut(knife, 0);
        }
    }


    // Track Star: full tracksuit and running shoes
    // Nerf on clothes, Two stones and alcohol tincture
    void EquipTheme_TrackStar(PlayerBase player)
    {
        ref array<string> cols = { "Blue","Green","Red","LightBlue","Black" };
        string col = cols[Math.RandomInt(0, cols.Count())];

        // Jacket heavier nerf; pants lighter nerf
        ReplaceSlotServer(player, "Body", "TrackSuitJacket_" + col, 0.15, 0.40);
        ReplaceSlotServer(player, "Legs", "TrackSuitPants_"  + col, 0.45, 0.60);

        // Shoes (pick, then replace)
        ref array<string> shoes = {
            "JoggingShoes_Black","JoggingShoes_Blue","JoggingShoes_Red","JoggingShoes_Violet","JoggingShoes_White"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.60);

        // 66% chance to wear sports glasses
        if (Math.RandomFloat01() < 0.66)
        {
            ref array<string> frames = { "SportGlasses_Black","SportGlasses_Blue","SportGlasses_Green","SportGlasses_Orange" };
            string framePick = frames[Math.RandomInt(0, frames.Count())];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        // Alcohol tincture (~2 uses)
        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); SetRandomHealth(alc); player.SetQuickBarEntityShortcut(alc, 1); }

        // Two pebbles
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }
    }


    // Sailor, same as base (BoneKnife variant)
    void EquipTheme_Sailor(PlayerBase player)
    {
        ReplaceSlotServer(player, "Body", "TelnyashkaShirt", 0.45, 0.75);
        if (player.FindAttachmentBySlotName("Body") && player.FindAttachmentBySlotName("Body").GetType() != "TelnyashkaShirt") {
            ACHLog("[ACH][Sailor] WARN Body not Telnyashka after replace");
        }
        // vanilla classname is NavalUniformPants
        ReplaceSlotServer(player, "Legs", "NavyUniformPants", 0.45, 0.75);
        if (player.FindAttachmentBySlotName("Legs") && player.FindAttachmentBySlotName("Legs").GetType() != "NavyUniformPants") {
            ACHLog("[ACH][Sailor] WARN Legs not NavalUniformPants after replace");
        }

        ref array<string> shoes = {
            "AthleticShoes_Black","AthleticShoes_Blue","AthleticShoes_Grey"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.75);

        // Damaged Bone Knife (no bandage)
        EntityAI knife = PutInAnyClothingCargo(player, "BoneKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }

        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }
    }


    // Thug, similar to base, bandanna added
    void EquipTheme_Thug(PlayerBase player)
    {
        // Palette
        ref array<string> bandanas = {
            "Bandana_BlackPattern", "Bandana_Blue", "Bandana_Greenpattern", "Bandana_Pink", "Bandana_PolkaPattern",
            "Bandana_RedPattern", "Bandana_Yellow"
        };
        // pick bandana
        string bandana = bandanas[Math.RandomInt(0, bandanas.Count())];

        // map bandana -> thug candidates, pants, shoes color (closest available)
        ref array<string> pantsChoices = new array<string>;
        ref array<string> shirtChoices = new array<string>;
        ref array<string> shoeChoices = new array<string>;

        switch (bandana)
        {
            case "Bandana_Blue":
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey", "Jeans_Blue", "Jeans_Green" };
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black", "TShirt_Beige" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White" };
                break;
            case "Bandana_Greenpattern":
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey", "Jeans_Brown" };
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White", "Sneakers_Green" };
                break;
            case "Bandana_Pink":
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey" };
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White", "Sneakers_Green" };
                break;
            case "Bandana_PolkaPattern":
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey", "Jeans_Green"};
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black", "TShirt_Beige", "TShirt_Green", "TShirt_OrangeWhiteStripes" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White" };
                break;
            case "Bandana_RedPattern":
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey", "Jeans_Blue", "Jeans_Brown"};
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black", "TShirt_Red", "TShirt_Green", "TShirt_RedBlackStripes" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White", "Sneakers_Red" };
                break;
            case "Bandana_Yellow":
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey", "Jeans_Blue", "Jeans_Brown", "Jeans_Green" };
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black", "TShirt_Green", "TShirt_Blue" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White", "Sneakers_Red" };
                break;
            case "Bandana_BlackPattern":
            default:
                pantsChoices = { "Jeans_BlueDark", "Jeans_Black", "Jeans_Grey", "Jeans_Blue", "Jeans_Brown", "Jeans_Green" };
                shirtChoices = { "TShirt_White", "TShirt_Grey", "TShirt_Black", "TShirt_Green", "TShirt_Blue",
                                "TShirt_OrangeWhiteStripes", "TShirt_RedBlackStripes", "TShirt_Red", "TShirt_Beige" };
                shoeChoices = { "Sneakers_Black", "Sneakers_Gray", "Sneakers_White", "Sneakers_Red" };
                break;
        }

        // Equip: outfit
        ReplaceSlotServer(player, "Headgear", bandana, 0.45, 0.75);
        ReplaceSlotServer(player, "Body", shirtChoices[Math.RandomInt(0, shirtChoices.Count())], 0.52, 0.75);
        ReplaceSlotServer(player, "Legs", pantsChoices[Math.RandomInt(0, pantsChoices.Count())], 0.45, 0.69);
        ReplaceSlotServer(player, "Feet", shoeChoices[Math.RandomInt(0, shoeChoices.Count())], 0.55, 0.75);
        ReplaceSlotServer(player, "Eyewear", "DesignerGlasses", 0.65, 0.80);

        // Items: bandage (worn-ish)
        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        EntityAI knife = PutInAnyClothingCargo(player, "SteakKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    // Hiker, similar to base, but hoodie, better shoes added
    void EquipTheme_Hiker(PlayerBase player)
    {
        ref array<string> hoodie = {
            "Hoodie_Black","Hoodie_Blue","Hoodie_Brown","Hoodie_Green","Hoodie_Grey","Hoodie_Red"
        };
        string hoodiePick = hoodie[Math.RandomInt(0, hoodie.Count())];
        ReplaceSlotServer(player, "Body", hoodiePick, 0.40, 0.60);

        ref array<string> pants = {
            "CanvasPants_Beige","CanvasPants_Blue","CanvasPants_Grey","CanvasPants_Red","CanvasPants_Violet"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        ReplaceSlotServer(player, "Legs", pantsPick, 0.45, 0.75);

        ref array<string> shoes = {
            "HikingBootsLow_Beige","HikingBootsLow_Black","HikingBootsLow_Blue","HikingBootsLow_Grey"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.35, 0.50);

        // Items: bandage (worn-ish)
        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        // Two pebbles
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }
    }


    // BallCapper, similar to base, based around wearing ballcap
    void EquipTheme_BallCapper(PlayerBase player)
    {
        ref array<string> hat = {
            "BaseballCap_Beige", "BaseballCap_Black", "BaseballCap_Blue", "BaseballCap_CMMG_Black",
            "BaseballCap_CMMG_Pink", "BaseballCap_Camo", "BaseballCap_Olive","BaseballCap_Pink", "BaseballCap_Red"
        };
        string hatPick = hat[Math.RandomInt(0, hat.Count())];
        ReplaceSlotServer(player, "Head", hatPick, 0.45, 0.75);

        ref array<string> shirts = {
            "TShirt_Beige", "TShirt_Black", "TShirt_Blue","TShirt_Green", "TShirt_Grey", "TShirt_OrangeWhiteStripes",
            "TShirt_Red", "TShirt_RedBlackStripes", "TShirt_White"
        };
        string shirtPick = shirts[Math.RandomInt(0, shirts.Count())];
        ReplaceSlotServer(player, "Body", shirtPick, 0.45, 0.75);

        ref array<string> pants = {
            "Breeches_Beetcheck", "Breeches_Beige", "Breeches_Black", "Breeches_Blackcheck", "Breeches_Blue",
            "Breeches_Browncheck", "Breeches_Green", "Breeches_Pink", "Breeches_Red", "Breeches_White"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        ReplaceSlotServer(player, "Legs", pantsPick, 0.45, 0.75);

        ref array<string> shoes = {
            "AthleticShoes_Black", "AthleticShoes_Blue", "AthleticShoes_Brown", "AthleticShoes_Green", "AthleticShoes_Grey",
            "Sneakers_Black", "Sneakers_Gray", "Sneakers_Green", "Sneakers_Red", "Sneakers_White"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.75, 1.0);

        // 42% chance to wear glasses
        if (Math.RandomFloat01() < 0.42) {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        // Items: bandage (worn-ish)
        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        // Two pebbles
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }
    }


    // FlatCapper, similar to base, based around wearing flatcap
    void EquipTheme_FlatCapper(PlayerBase player, bool male)
    {
        ref array<string> hat = {
            "FlatCap_Black", "FlatCap_BlackCheck", "FlatCap_Blue", "FlatCap_Brown", "FlatCap_BrownCheck", "FlatCap_Grey",
            "FlatCap_GreyCheck", "FlatCap_Red"
        };
        string hatPick = hat[Math.RandomInt(0, hat.Count())];
        ReplaceSlotServer(player, "Head", hatPick, 0.45, 0.75);

        if (male)
        {
            ref array<string> shirts = {
                "TShirt_Beige", "TShirt_Black", "TShirt_Blue","TShirt_Green", "TShirt_Grey", "TShirt_OrangeWhiteStripes",
                "TShirt_Red", "TShirt_RedBlackStripes", "TShirt_White"
            };
            string shirtPick = shirts[Math.RandomInt(0, shirts.Count())];
            ReplaceSlotServer(player, "Body", shirtPick, 0.45, 0.75);

        } else {
            ref array<string> femShirts = {
                "Blouse_Blue", "Blouse_Green", "Blouse_Violet", "Blouse_White"
            };
            string femShirtPick = femShirts[Math.RandomInt(0, femShirts.Count())];
            ReplaceSlotServer(player, "Body", femShirtPick, 0.45, 0.75);
        }

        ref array<string> pants = {
            "Breeches_Beetcheck", "Breeches_Beige", "Breeches_Black", "Breeches_Blackcheck", "Breeches_Blue",
            "Breeches_Browncheck", "Breeches_Green", "Breeches_Pink", "Breeches_Red", "Breeches_White"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        ReplaceSlotServer(player, "Legs", pantsPick, 0.45, 0.75);

        ref array<string> shoes = {
            "AthleticShoes_Black", "AthleticShoes_Blue", "AthleticShoes_Brown", "AthleticShoes_Green", "AthleticShoes_Grey"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.75, 1.0);

        // 42% chance to wear glasses
        if (Math.RandomFloat01() < 0.42) {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        // Items: bandage (worn-ish)
        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        // Two pebbles
        for (int i = 0; i < 2; i++)
        {
            EntityAI stone = PutInAnyClothingCargo(player, "SmallStone");
            if (stone) SetRandomHealth(stone);
        }
    }


    // LabRat Mix of doc and professional
    void EquipTheme_LabRat(PlayerBase player)
    {
        ReplaceSlotServer(player, "Body", "LabCoat", 0.25, 0.40);

        ref array<string> pants = {
            "SlacksPants_Beige","SlacksPants_Black","SlacksPants_Blue","SlacksPants_Brown",
            "SlacksPants_DarkGrey","SlacksPants_Khaki","SlacksPants_LightGrey","SlacksPants_White"
        };
        string pantsPick = pants[Math.RandomInt(0, pants.Count())];
        ReplaceSlotServer(player, "Legs", pantsPick, 0.45, 0.70);

        ref array<string> shoes = {
            "DressShoes_Beige","DressShoes_Black","DressShoes_Brown","DressShoes_Sunburst","DressShoes_White"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.45, 0.70);

        // 88% chance glasses
        if (Math.RandomFloat01() < 0.88) {
            string frames[] = { "ThickFramesGlasses","ThinFramesGlasses" };
            string framePick = frames[Math.RandomInt(0, 2)];
            ReplaceSlotServer(player, "Eyewear", framePick, 0.65, 0.80);
        }

        // ~2-use alcohol
        ItemBase alc = ItemBase.Cast(PutInAnyClothingCargo(player, "DisinfectantAlcohol"));
        if (alc) { alc.SetQuantity(20); SetWorn(alc); player.SetQuickBarEntityShortcut(alc, 1); }

        // Damaged BoneKnife (no bandage)
        EntityAI knife = PutInAnyClothingCargo(player, "BoneKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    // Yes... a Dress
    void EquipTheme_YesADress(PlayerBase player)
    {
        ref array<string> dress = {
            "MiniDress_BlueChecker", "MiniDress_BlueWithDots", "MiniDress_BrownChecker", "MiniDress_GreenChecker",
            "MiniDress_Pink", "MiniDress_PinkChecker", "MiniDress_RedChecker", "MiniDress_WhiteChecker"
        };
        string dressPick = dress[Math.RandomInt(0, dress.Count())];
        ReplaceSlotServer(player, "Body", dressPick, 0.75, 1.0);

        ref array<string> shoes = {
            "Sneakers_Black", "Sneakers_Gray", "Sneakers_Green", "Sneakers_Red", "Sneakers_White"
        };
        string shoePick = shoes[Math.RandomInt(0, shoes.Count())];
        ReplaceSlotServer(player, "Feet", shoePick, 0.75, 1.0);

        ClearSlotServer(player, "Legs");

        ref array<string> kidBags = { "ChildBag_Blue","ChildBag_Green","ChildBag_Red" };
        string bagPick = kidBags[Math.RandomInt(0, kidBags.Count())];
        ReplaceSlotServer(player, "Back", bagPick, 0.75, 1.0);

        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        EntityAI knife = PutInAnyClothingCargo(player, "SteakKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(knife, 0); }
    }


    // Chalk: intentionally no-op. Keep whatever vanilla spawned.
    // Common items (armband/chemlight/UnknownFoodCan) are added after the theme.
    void EquipTheme_Chalk(PlayerBase player, bool male)
    {
        // Items: bandage (worn-ish)
        EntityAI band = PutInAnyClothingCargo(player, "BandageDressing");
        if (band) { SetPristine(band); player.SetQuickBarEntityShortcut(band, 1); }

        EntityAI knife = PutInAnyClothingCargo(player, "SteakKnife");
        if (knife) { SetLowHealth(knife); player.SetQuickBarEntityShortcut(band, 0); }

        // Optional safety (commented): if a slot is somehow empty, you could equip basics.
        // if (!player.FindAttachmentBySlotName("Body")) ReplaceSlotServer(player, "Body", "TShirt_Black", 0.45, 0.70);
        // if (!player.FindAttachmentBySlotName("Legs")) ReplaceSlotServer(player, "Legs", "Jeans_Blue", 0.45, 0.70);
        // if (!player.FindAttachmentBySlotName("Feet")) ReplaceSlotServer(player, "Feet", "Sneakers_White", 0.45, 0.70);
    }


    // ---------- Vanilla hooks ----------
    override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
    {
        Entity playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
        Class.CastTo(m_player, playerEnt);
        GetGame().SelectPlayer(identity, m_player);
        return m_player;
    }

    // For Cyclic spawning - Testing or some events
    void EquipNextTheme(PlayerBase player)
    {
        int idx = mThemeIndex;
        const int THEME_COUNT = 17; // 0..16 inclusive

        switch (idx % THEME_COUNT)
        {
            case 0:  EquipTheme_Cop(player);                                      break;
            case 1:  EquipTheme_Prisoner(player);                                 break; // if you want it
            case 2:  EquipTheme_Professional(player, player.IsMale());            break;
            case 3:  EquipTheme_SorryNotSorry(player);                            break;
            case 4:  EquipTheme_Paramedic(player);                                break;
            case 5:  EquipTheme_Nurse(player, player.IsMale());                   break;
            case 6:  EquipTheme_Doctor(player);                                   break;
            case 7:  EquipTheme_Mummy(player);                                    break;
            case 8:  EquipTheme_TrackStar(player);                                break;
            case 9:  EquipTheme_Sailor(player);                                   break;
            case 10: EquipTheme_Thug(player);                                     break;
            case 11: EquipTheme_Hiker(player);                                    break;
            case 12: EquipTheme_BallCapper(player);                               break;
            case 13: EquipTheme_FlatCapper(player, player.IsMale());              break;
            case 14: EquipTheme_LabRat(player);                                   break;
            case 15: EquipTheme_YesADress(player);                                break;
            case 16: EquipTheme_Chalk(player, player.IsMale());                   break;
            default: /* unreachable due to modulus */                             break;
        }

        mThemeIndex = mThemeIndex + 1;
        SaveThemeIndex();
    }


    override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
    {
        // Server only. Then let vanilla finish dressing first.
        if (!GetGame().IsServer()) return;
        super.StartingEquipSetup(player, clothesChosen);
        // EquipNextTheme(player);
        bool male = player.IsMale();

        // Weights
        float wWellie           = 1.0;
        float wShaman           = 1.0;
        float wCowboy           = 1.0;
        float wPrisoner         = 0.9;
        float wCop              = 0.75;
        float wProfessional     = 0.9;
        float wSorryNotSorry    = 1.0;
        float wParamedic        = 0.4;
        float wNurse            = 0.8;
        float wDoctor           = 0.6;
        float wMummy            = 1.0;
        float wTrackStar        = 0.8;
        float wSailor           = 1.0;
        float wThug             = 1.0;
        float wHiker            = 0.8;
        float wBallCapper       = 0.95;
        float wFlatCapper       = 0.95;
        float wLabRat           = 0.75;
        float wYesADress        = 1.0;
        float wChalk            = 0.7;

        // float wWellie           = 0.0;
        // float wShaman           = 0.0;
        // float wCowboy           = 0.0;
        // float wPrisoner         = 0.0;
        // float wCop              = 0.0;
        // float wProfessional     = 0.0;
        // float wSorryNotSorry    = 1.0;
        // float wParamedic        = 0.0;
        // float wNurse            = 0.0;
        // float wDoctor           = 0.0;
        // float wMummy            = 0.0;
        // float wTrackStar        = 0.0;
        // float wSailor           = 0.0;
        // float wThug             = 0.0;
        // float wHiker            = 0.0;
        // float wBallCapper       = 0.0;
        // float wFlatCapper       = 0.0;
        // float wLabRat           = 0.0;
        // float wYesADress        = 0.0;
        // float wChalk            = 0.0;

        // Include ALL weights in total
        float total = wWellie + wShaman + wCowboy + wPrisoner + wCop + wProfessional + wSorryNotSorry + wParamedic + wNurse;
        total += (wDoctor + wMummy + wTrackStar + wSailor + wThug + wHiker + wBallCapper + wFlatCapper + wLabRat + wYesADress + wChalk);
        // Failsafe
      //  if (total <= 0.0) { EquipTheme_Chalk(player, male); GiveCommonItems(player); return; }

        // Roll
        float roll = Math.RandomFloat(0.0, total);

        // Cumulative thresholds
        float t0  = wWellie;
        float t1  = t0  + wShaman;
        float t2  = t1  + wCowboy;
        float t3  = t2  + wPrisoner;
        float t4  = t3  + wCop;
        float t5  = t4  + wProfessional;
        float t6  = t5  + wSorryNotSorry;
        float t7  = t6  + wParamedic;
        float t8  = t7  + wNurse;
        float t9  = t8  + wDoctor;
        float t10 = t9  + wMummy;
        float t11 = t10 + wTrackStar;
        float t12 = t11 + wSailor;
        float t13 = t12 + wThug;
        float t14 = t13 + wHiker;
        float t15 = t14 + wBallCapper;
        float t16 = t15 + wFlatCapper;
        float t17 = t16 + wLabRat;
        float t18 = t17 + wYesADress;
        // t19 would be + wChalk == total
        bool skipArmband = false;

        if      (roll <= t0)  { EquipTheme_WellieGuy(player); skipArmband = true; }
        else if (roll <= t1)  { EquipTheme_Shaman(player); }
        else if (roll <= t2)  { EquipTheme_Cowboy(player, male); }
        else if (roll <= t3)  { EquipTheme_Prisoner(player); }
        else if (roll <= t4)  { EquipTheme_Cop(player); }
        else if (roll <= t5)  { EquipTheme_Professional(player, male); }
        else if (roll <= t6)  { EquipTheme_SorryNotSorry(player); }
        else if (roll <= t7)  { EquipTheme_Paramedic(player); }
        else if (roll <= t8)  { EquipTheme_Nurse(player, male); }
        else if (roll <= t9)  { EquipTheme_Doctor(player); }
        else if (roll <= t10) { EquipTheme_Mummy(player); }
        else if (roll <= t11) { EquipTheme_TrackStar(player); }
        else if (roll <= t12) { EquipTheme_Sailor(player); }
        else if (roll <= t13) { EquipTheme_Thug(player); }
        else if (roll <= t14) { EquipTheme_Hiker(player); }
        else if (roll <= t15) { EquipTheme_BallCapper(player); }
        else if (roll <= t16) { EquipTheme_FlatCapper(player, male); }
        else if (roll <= t17) { EquipTheme_LabRat(player); }
        else if (roll <= t18) { EquipTheme_YesADress(player); }
        else                   { EquipTheme_Chalk(player, male); }

        //DumpWear(player, "after Wellie");

        // Stats (same as original)
        player.GetStatWater().Set(750);
        player.GetStatEnergy().Set(1000);
        player.SetTemporaryResistanceToAgent(eAgents.INFLUENZA, 900);

        // Universal items: chemlight + damaged UnknownFoodCan + (maybe) armband
        // (Assumes you’ve got GiveCommonItems(player, bool skipArmband) defined)
        if (skipArmband)
            GiveCommonItemsEx(player, skipArmband);
        else
            GiveCommonItems(player);
    }


    // Common to ALL characters (armband except future WellieGuy)
    void GiveCommonItems(PlayerBase player) { GiveCommonItemsEx(player, false); }
    void GiveCommonItemsEx(PlayerBase player, bool skipArmband)
    {
        // Random chemlight (any health)
        ref array<string> chems = { "Chemlight_White","Chemlight_Yellow","Chemlight_Green","Chemlight_Red" };
        EntityAI chem = PutInAnyClothingCargo(player, chems[Math.RandomInt(0, chems.Count())]);
        ItemBase chemIB = ItemBase.Cast(chem);
        if (chemIB) { SetRandomHealth(chemIB); player.SetQuickBarEntityShortcut(chemIB, 2); }

        // Damaged UnknownFoodCan
        ItemBase can = ItemBase.Cast(PutInAnyClothingCargo(player, "UnknownFoodCan"));
        if (can) { can.SetHealth01("", "", Math.RandomFloatInclusive(0.45, 0.75)); }

        // Armband (simple random for now)
        if (!skipArmband) GiveRandomArmband(player);

        // Everyone gets a radio
        bool giveRadio = true;
        if (giveRadio)
        {
            // Give radio + battery + power it on
            ItemBase radio = ItemBase.Cast(player.GetInventory().CreateInInventory("PersonalRadio"));
            if (radio)
            {
                radio.SetHealth01("", "", 1.0);

                // best-effort: attach a 9V (no-op if already attached or slot is full)
                ItemBase.Cast(radio.GetInventory().CreateAttachment("Battery9V"));

                // Power it via energy manager (paramless; safe across versions)
                auto em = radio.GetCompEM();
                if (em)
                {
                    if (!em.IsSwitchedOn() && em.CanSwitchOn())
                        em.SwitchOn();
                }

                player.SetQuickBarEntityShortcut(radio, 3);
            }
        }
    }
};


Mission CreateCustomMission(string path)
{
    return new CustomMission();
}
