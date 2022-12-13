using System;
using System.Runtime.InteropServices;

namespace TASE2.Library.Server
{
    /// <summary>
    /// Represents a data set in the server data model
    /// </summary>
    public class DataSet
    {
        [DllImport("tase2", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Tase2_DataSet_addEntry(IntPtr self, IntPtr domain, string variableName);

        private IntPtr self;

        private string name;

        public string Name
        {
            get
            {
                return name;
            }
        }

        internal DataSet(IntPtr dataSetPtr, string name)
        {
            self = dataSetPtr;
            this.name = name;
        }

        /// <summary>
        /// Adds a new data set entry.
        /// </summary>
        /// <param name="domain">the domain of the new entry</param>
        /// <param name="variableName">the variable name of the new entry</param>
        public void AddEntry(Domain domain, string variableName)
        {
            Tase2_DataSet_addEntry(self, domain.Self, variableName);
        }
    }
}

