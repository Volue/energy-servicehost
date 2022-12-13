using System;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Security.Cryptography.X509Certificates;
using System.Text;

namespace Powel.Icc.Services.DataContracts
{
    /// <summary>
    /// Summary description for Quality.
    /// </summary>
    public enum ValidationMethods
    {
        [Description("All")]
        None = 0,
        [Description("AbsLimit")]
        AbsoluteLimit = 1,
        [Description("DeltaLimit")]
        DeltaLimit = 2,
        [Description("DeltaLimitExtreme")]
        DeltaEpsilon = 3,
        [Description("RepeatValue")]
        RepeatValue = 4,
        [Description("Compare with validation meter")]
        ControlMeter = 5,
        [Description("Periodic volume")]
        MeterSum = 6,
        [Description("Control high tariff period")]
        ControlHighTariffPeriod = 28,
        [Description("Control tariff")]
        ControlTariff = 29,
        [Description("Control accessible energy")]
        ControlAccessibleEnergy = 30,
        [Description("Control import")]
        ControlImport = 31
    }

    public enum CorrectionMethods
    {
        [Description("All")]
        None = 0,
        [Description("ConstantValue")]
        Constant = 1,
        [Description("CopyValue")]
        Copy = 2,
        [Description("Interpolate")]
        Interpolate = 3,
        [Description("Extrapolate")]
        Extrapolate = 4,
        [Description("AverageValue")]
        Mean = 5,
        [Description("Energy")]
        Energy = 8,
        [Description("Mean delta T")]
        MeanDeltaT = 9,
        [Description("Meter sum")]
        MeterSum = 10
    }
    public struct Quality : IEquatable<Quality>
    {
        BitVector32 exposedQuality;
        private static BitVector32.Section correctionMethodSection = BitVector32.CreateSection(31);
        private static BitVector32.Section validationMethodSection = BitVector32.CreateSection(31, correctionMethodSection);
        private static BitVector32.Section customerMethodSection = BitVector32.CreateSection(31, validationMethodSection);
        private static BitVector32.Section deletedSection = BitVector32.CreateSection(7, customerMethodSection);

        private static BitVector32.Section inspectedSection = BitVector32.CreateSection(1, deletedSection);
        private static BitVector32.Section defaultSection = BitVector32.CreateSection(1, inspectedSection);
        private static BitVector32.Section virtualPointSection = BitVector32.CreateSection(1, defaultSection);

        private static BitVector32.Section estimatedSection = BitVector32.CreateSection(1, virtualPointSection);
        private static BitVector32.Section moveFlagSection = BitVector32.CreateSection(1, estimatedSection);
        private static BitVector32.Section acceptedSection = BitVector32.CreateSection(1, moveFlagSection);
        private static BitVector32.Section lockedSection = BitVector32.CreateSection(1, acceptedSection);
        private static BitVector32.Section suspectSection = BitVector32.CreateSection(1, lockedSection);
        private static BitVector32.Section missingSection = BitVector32.CreateSection(1, suspectSection);
        private static BitVector32.Section manualSection = BitVector32.CreateSection(1, missingSection);
        private static BitVector32.Section correctedSection = BitVector32.CreateSection(1, manualSection);
        private static BitVector32.Section validatedSection = BitVector32.CreateSection(1, correctedSection);
        private static BitVector32.Section notOKSection = BitVector32.CreateSection(1, validatedSection);

        BitVector32 internalQuality;
        private static BitVector32.Section modifiedSection = BitVector32.CreateSection(1);

       

        

        public Quality(Quality q)
        {
            this.exposedQuality = q.exposedQuality;
            this.internalQuality = q.internalQuality;
        }
        public void CopyFrom(Quality q)
        {
            exposedQuality = q.exposedQuality;
            internalQuality = q.internalQuality;
        }

        public Quality(int quality)
        {
            this.exposedQuality = new BitVector32(quality);
            this.internalQuality = new BitVector32(0);
        }

        public int Data
        {
            get { return exposedQuality.Data; }
        }

        public Quality SetCorrection(CorrectionMethods correctionMethod)
        {
            return SetExposedSection(this, correctionMethodSection, (int)correctionMethod);
        }

        public Quality SetValidation(ValidationMethods validationMethod)
        {
            return SetExposedSection(this, validationMethodSection, (int)validationMethod);
        }

        public Quality SetCustom(int customStatus)
        {
            return SetExposedSection(this, customerMethodSection, customStatus);
        }

        public CorrectionMethods Correction
        {
            get { return (CorrectionMethods)exposedQuality[correctionMethodSection]; }
        }

        public ValidationMethods Validation
        {
            get { return (ValidationMethods)exposedQuality[validationMethodSection]; }
        }

        public int Custom
        {
            get { return exposedQuality[customerMethodSection]; }
        }

        public static Quality OK
        {
            get { return new Quality(0); }
        }

        public static Quality Estimated
        {
            get { return SetExposedFlag(new Quality(), estimatedSection, true); }
        }

        public static Quality Accepted
        {
            get { return SetExposedFlag(new Quality(), acceptedSection, true); }
        }

        public static Quality Locked
        {
            get { return SetExposedFlag(new Quality(), lockedSection, true); }
        }

        public static Quality Suspect
        {
            get { return SetExposedFlag(new Quality(), suspectSection, true); }
        }

        public static Quality Missing
        {
            get { return SetExposedFlag(new Quality(), missingSection, true); }
        }

        public static Quality Manual
        {
            get { return SetExposedFlag(new Quality(), manualSection, true); }
        }

        public static Quality Corrected
        {
            get { return SetExposedFlag(new Quality(), correctedSection, true); }
        }

        public static Quality Validated
        {
            get { return SetExposedFlag(new Quality(), validatedSection, true); }
        }

        public static Quality NotOK
        {
            get { return SetExposedFlag(new Quality(), notOKSection, true); }
        }

        public static Quality Modified
        {
            get { return SetInternalFlag(new Quality(), modifiedSection, true); }
        }

        public static Quality Default
        {
            get { return SetExposedFlag(new Quality(), defaultSection, true); }
        }


        public Quality SetEstimated(bool isEstimated)
        {
            return SetExposedFlag(this, estimatedSection, isEstimated);
        }

        public Quality SetAccepted(bool isAccepted)
        {
            return SetExposedFlag(this, acceptedSection, isAccepted);
        }

        public Quality SetLocked(bool isLocked)
        {
            return SetExposedFlag(this, lockedSection, isLocked);
        }

        public Quality SetSuspect(bool isSuspect)
        {
            return SetExposedFlag(this, suspectSection, isSuspect);
        }

        public Quality SetMissing(bool isMissing)
        {
            return SetExposedFlag(this, missingSection, isMissing);
        }

        public Quality SetManual(bool isManual)
        {
            return SetExposedFlag(this, manualSection, isManual);
        }

        public Quality SetCorrected(bool isCorrected)
        {
            return SetExposedFlag(this, correctedSection, isCorrected);
        }

        public Quality SetValidated(bool isValidated)
        {
            return SetExposedFlag(this, validatedSection, isValidated);
        }

        public Quality SetNotOK(bool isNotOK)
        {
            return SetExposedFlag(this, notOKSection, isNotOK);
        }

        public Quality SetModified(bool isModified)
        {
            return SetInternalFlag(this, modifiedSection, isModified);
        }

        public Quality SetDefault(bool isDefault)
        {
            return SetInternalFlag(this, defaultSection, isDefault);
        }

        public Quality SetVirtualPoint(bool isVirtualPoint)
        {
            return SetInternalFlag(this, virtualPointSection, isVirtualPoint);
        }

        public Quality ExposeAsVirtualPoint(bool isVirtualPoint)
        {
            return SetExposedFlag(this, virtualPointSection, isVirtualPoint);
        }

        public bool isEstimated
        {
            get { return GetExposedFlag(estimatedSection); }
        }

        public bool IsAccepted
        {
            get { return GetExposedFlag(acceptedSection); }
        }

        public bool IsLocked
        {
            get { return GetExposedFlag(lockedSection); }
        }

        public bool IsSuspect
        {
            get { return GetExposedFlag(suspectSection); }
        }

        public bool IsMissing
        {
            get { return GetExposedFlag(missingSection); }
        }

        public bool IsManual
        {
            get { return GetExposedFlag(manualSection); }
        }

        public bool IsCorrected
        {
            get { return GetExposedFlag(correctedSection); }
        }

        public bool IsValidated
        {
            get { return GetExposedFlag(validatedSection); }
        }

        public bool IsNotOK
        {
            get { return GetExposedFlag(notOKSection); }
        }

        public bool IsModified
        {
            get { return GetInternalFlag(modifiedSection); }
        }

        public bool IsDefault
        {
            get { return GetExposedFlag(defaultSection); }
        }

        public bool IsVirtualPoint
        {
            get { return GetExposedFlag(virtualPointSection); }
        }

        static Quality SetExposedSection(Quality originalQuality, BitVector32.Section section, int value)
        {
            Quality q = new Quality(originalQuality);
            q.exposedQuality[section] = value;
            return q;
        }

        static Quality SetInternalSection(Quality originalQuality, BitVector32.Section section, int value)
        {
            Quality q = new Quality(originalQuality);
            q.internalQuality[section] = value;
            return q;
        }

        static Quality SetExposedFlag(Quality originalQuality, BitVector32.Section section, bool value)
        {
            return SetExposedSection(originalQuality, section, Convert.ToInt32(value));
        }

        static Quality SetInternalFlag(Quality originalQuality, BitVector32.Section section, bool value)
        {
            return SetInternalSection(originalQuality, section, Convert.ToInt32(value));
        }

        bool GetExposedFlag(BitVector32.Section section)
        {
            return Convert.ToBoolean(exposedQuality[section]);
        }

        bool GetInternalFlag(BitVector32.Section section)
        {
            return Convert.ToBoolean(internalQuality[section]);
        }

        public static explicit operator int(Quality quality)
        {
            return quality.exposedQuality.Data;
        }

        public static Quality operator |(Quality q1, Quality q2)
        {
            Quality q = new Quality();
            q.exposedQuality = new BitVector32(q1.exposedQuality.Data | q2.exposedQuality.Data);
            q.internalQuality = new BitVector32(q1.internalQuality.Data | q2.internalQuality.Data);
            return q;
        }

        public static Quality operator &(Quality q1, Quality q2)
        {
            Quality q = new Quality();
            q.exposedQuality = new BitVector32(q1.exposedQuality.Data & q2.exposedQuality.Data);
            q.internalQuality = new BitVector32(q1.internalQuality.Data & q2.internalQuality.Data);
            return q;
        }


        public string FullStatus
        {
            get
            {
                return GetStatusString();

            }
        }

        public string GetStatusString()
        {
            StringBuilder result = new StringBuilder("................."); // len = 17
            int method = 0;
            if (exposedQuality[notOKSection] > 0)
                result.Remove(0, 1).Insert(0, "!");
            if (exposedQuality[acceptedSection] > 0)
                result.Remove(1, 1).Insert(1, "A");
            if (exposedQuality[suspectSection] > 0)
                result.Remove(2, 1).Insert(2, "S");
            if (exposedQuality[missingSection] > 0)
                result.Remove(3, 1).Insert(3, "X");
            if (exposedQuality[lockedSection] > 0)
                result.Remove(4, 1).Insert(4, "L");
            if (exposedQuality[manualSection] > 0)
                result.Remove(5, 1).Insert(5, "M");
            else if (exposedQuality[estimatedSection] > 0)
                result.Remove(5, 1).Insert(5, "m");

            if (exposedQuality[validatedSection] > 0)
            {
                result.Remove(6, 1).Insert(6, "V");
                method = exposedQuality[validationMethodSection];
                result.Remove(7, 2).Insert(7, method.ToString("D2"));
            }

            if (exposedQuality[correctedSection] > 0)
            {
                result.Remove(9, 1).Insert(9, "C");
                method = exposedQuality[correctionMethodSection];
                result.Remove(10, 2).Insert(10, method.ToString("D2"));
            }

            if (exposedQuality[customerMethodSection] > 0)
            {
                result.Remove(12, 1).Insert(12, "E");
                method = exposedQuality[customerMethodSection];
                result.Insert(13, method.ToString("D2"));
            }

            if (exposedQuality[defaultSection] > 0)
                result.Remove(15, 1).Insert(15, "D");


            if (exposedQuality[virtualPointSection] > 0)
                result.Remove(16, 1).Insert(16, "~");

            //TODO: Inspected 

            return result.ToString();

        }

        public bool Equals(Quality other)
        {
            return exposedQuality.Data == other.exposedQuality.Data
                        && internalQuality.Data == other.internalQuality.Data;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Quality))
            {
                return false;
            }

            return Equals((Quality)obj);
        }

        public static bool operator ==(Quality left, Quality right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(Quality left, Quality right)
        {
            return !(left == right);
        }
    }
}
