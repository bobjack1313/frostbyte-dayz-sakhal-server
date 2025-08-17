// scripts/4_World/Themes/ThemeRegistry.c
class ThemeRegistry
{
    // ONLY include themes that exist to avoid compile errors.
    static ref array<string> NAMES()
    {
        ref array<string> a = new array<string>;
        a.Insert("Chalk");
        a.Insert("WellieGuy");
        a.Insert("Shaman");
        a.Insert("Cowboy");
        a.Insert("Prisoner");
        a.Insert("Cop");
        a.Insert("Professional");
        a.Insert("SorryNotSorry");
        a.Insert("Paramedic");
        a.Insert("Nurse");
        a.Insert("Doctor");
        a.Insert("Mummy");
        a.Insert("TrackStar");
        a.Insert("Sailor");
        a.Insert("Thug");
        a.Insert("Hiker");
        a.Insert("FlatCapper");
        a.Insert("LabRat");
        a.Insert("YesADress");
        a.Insert("ConstructionWorker");
        a.Insert("BallCapper");
        return a;
    }

    // --- Random pick over NAMES()
    static void ApplyRandom(PlayerBase p, bool male)
    {
        ref array<string> names = NAMES();
        if (!names || names.Count() == 0) { Theme_Chalk.Apply(p, male); return; }
        int i = Math.RandomInt(0, names.Count());
        ApplyByName(p, male, names[i]);
    }

    // --- Weighted pick: aligned arrays of names/weights (names must be a subset of NAMES())
    static void ApplyWeighted(PlayerBase p, bool male, array<string> names, array<float> weights)
    {
        if (!names || !weights || names.Count() == 0 || weights.Count() != names.Count())
        {
            ApplyRandom(p, male);
            return;
        }

        float total = 0.0;
        for (int i = 0; i < weights.Count(); i++) total += Math.Max(weights[i], 0.0);
        if (total <= 0.0) { Theme_Chalk.Apply(p, male); return; }

        float roll = Math.RandomFloat(0.0, total);
        float acc = 0.0;
        for (int j = 0; j < names.Count(); j++)
        {
            float w = Math.Max(weights[j], 0.0);
            acc += w;
            if (roll <= acc)
            {
                ApplyByName(p, male, names[j]);
                return;
            }
        }
        Theme_Chalk.Apply(p, male);
    }

    // --- Cyclic pick over NAMES(); returns the next index to persist
    static int ApplyCyclic(PlayerBase p, bool male, int index)
    {
        ref array<string> names = NAMES();
        if (!names || names.Count() == 0) { Theme_Chalk.Apply(p, male); return 0; }

        int i = index % names.Count();
        if (i < 0) i = 0;
        ApplyByName(p, male, names[i]);
        return (i + 1);
    }

    // --- Dispatcher
    static void ApplyByName(PlayerBase p, bool male, string name)
    {
        // Keep these if-branches ONLY for implemented themes
        if (name == "Chalk")             { Theme_Chalk.Apply(p, male); return; }
        if (name == "WellieGuy")         { Theme_WellieGuy.Apply(p, male); return; }
        if (name == "Shaman")            { Theme_Shaman.Apply(p, male); return; }
        if (name == "Cowboy")            { Theme_Cowboy.Apply(p, male); return; }
        if (name == "Prisoner")          { Theme_Prisoner.Apply(p, male); return; }
        if (name == "Cop")               { Theme_Cop.Apply(p, male); return; }
        if (name == "Professional")      { Theme_Professional.Apply(p, male); return; }
        if (name == "SorryNotSorry")     { Theme_SorryNotSorry.Apply(p, male); return; }
        if (name == "Paramedic")         { Theme_Paramedic.Apply(p, male); return; }
        if (name == "Nurse")             { Theme_Nurse.Apply(p, male); return; }
        if (name == "Doctor")            { Theme_Doctor.Apply(p, male); return; }
        if (name == "Mummy")             { Theme_Mummy.Apply(p, male); return; }
        if (name == "TrackStar")         { Theme_TrackStar.Apply(p, male); return; }
        if (name == "Sailor")            { Theme_Sailor.Apply(p, male); return; }
        if (name == "Thug")              { Theme_Thug.Apply(p, male); return; }
        if (name == "Hiker")             { Theme_Hiker.Apply(p, male); return; }
        if (name == "FlatCapper")        { Theme_FlatCapper.Apply(p, male); return; }
        if (name == "LabRat")            { Theme_LabRat.Apply(p, male); return; }
        if (name == "YesADress")         { Theme_YesADress.Apply(p, male); return; }
        if (name == "ConstructionWorker"){ Theme_ConstructionWorker.Apply(p, male); return; }
        if (name == "BallCapper")        { Theme_BallCapper.Apply(p, male); return; }

        Theme_Chalk.Apply(p, male);
    }
}
