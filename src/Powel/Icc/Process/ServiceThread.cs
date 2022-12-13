using System;
using System.Threading;

namespace Powel.Icc.Process
{
	public class ServiceThread
	{
	    readonly ServiceIterationBase iteration;
	    readonly int iterationSeconds;
	    readonly int logsAgeInMinutes;
		//int errorWait;
		//int minimumErrorWait;

		public ServiceThread(ServiceIterationBase iteration, int iterationSeconds, int logsAgeInMinutes)
		{
			this.iteration = iteration;
			this.iterationSeconds = iterationSeconds;
			this.logsAgeInMinutes = logsAgeInMinutes;
		}

		public void StartProcess()
		{
			var t = new Thread(RunProcess);
			t.Start();
		}

		private void RunProcess()
		{
			try
			{
				while (!iteration.StopRequested())
				{
					try
					{
						bool possiblyMoreWork;

						iteration.RunIteration(out possiblyMoreWork);

						// We will only sleep long if there's nothing to do.
					    if (iteration.StopRequested())
					        break;
						if (possiblyMoreWork)
							Thread.Sleep(1);
						else
							Thread.Sleep(iterationSeconds*1000);

						iteration.RecycleLog(logsAgeInMinutes);
					}
					catch (Exception ex)
					{
						iteration.CriticalLog(ex);
					}
				}
			}
			finally
			{
				iteration.IsRunning = false;

			    iteration.Dispose();
			}
		}
	}
}
