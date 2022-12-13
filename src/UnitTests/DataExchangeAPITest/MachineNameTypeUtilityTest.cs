using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace Powel.Icc.Messaging.DataExchangeManager.DataExchangeApiTest
{
    [TestFixture]
    public class MachineNameTypeUtilityTest
    {
        [Test]
        public void DetermineMachineNameType_MachineNameIsDot_IsLocal()
        {
            // Assign

            const string machineName = ".";

            // Act

            var machineNameType = MachineNameTypeUtility.DetermineMachineNameType(machineName);

            // Assert

            Assert.AreEqual(MachineNameType.Local, machineNameType);
        }

        [Test]
        public void DetermineMachineNameType_MachineNameIsNull_IsLocal()
        {
            // Assign

            const string machineName = null;

            // Act

            var machineNameType = MachineNameTypeUtility.DetermineMachineNameType(machineName);

            // Assert

            Assert.AreEqual(MachineNameType.Local, machineNameType);
        }

        [Test]
        public void DetermineMachineNameType_MachineNameIsEmpty_IsLocal()
        {
            // Assign

            const string machineName = "";

            // Act

            var machineNameType = MachineNameTypeUtility.DetermineMachineNameType(machineName);

            // Assert

            Assert.AreEqual(MachineNameType.Local, machineNameType);
        }

        [Test]
        public void DetermineMachineNameType_MachineNameIsIpAddress_IsTcp()
        {
            // Assign

            const string machineName = "192.168.0.0";

            // Act

            var machineNameType = MachineNameTypeUtility.DetermineMachineNameType(machineName);

            // Assert

            Assert.AreEqual(MachineNameType.Tcp, machineNameType);
        }

        [Test]
        public void DetermineMachineNameType_MachineNameIsNamedMachine_IsOs()
        {
            // Assign

            const string machineName = "dummy";

            // Act

            var machineNameType = MachineNameTypeUtility.DetermineMachineNameType(machineName);

            // Assert

            Assert.AreEqual(MachineNameType.Os, machineNameType);
        }
    }
}
