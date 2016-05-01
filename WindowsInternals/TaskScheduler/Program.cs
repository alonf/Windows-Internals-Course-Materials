using System;
using System.Diagnostics;
using System.IO;

namespace TaskScheduler
{
    class Program
    {
        static void Main()
        {
            ITaskService scheduler = new TaskScheduler();
            scheduler.Connect();

            ITaskFolder taskFolder = scheduler.GetFolder("\\");

            ITaskDefinition newTask = scheduler.NewTask(0);
            var action = (IExecAction)newTask.Actions.Create(_TASK_ACTION_TYPE.TASK_ACTION_EXEC);
            action.Path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "Calc.exe");
            action.WorkingDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            action.Id = "My Calc Task";
            newTask.Settings.Enabled = true;

            //10 hours to finish execution
            newTask.Settings.ExecutionTimeLimit = "PT5M";
            newTask.Settings.WakeToRun = true;
            newTask.Settings.StartWhenAvailable = true;

            var timeTrigger = newTask.Triggers.Create(_TASK_TRIGGER_TYPE2.TASK_TRIGGER_DAILY) as IDailyTrigger;
            Debug.Assert(timeTrigger != null);

            timeTrigger.Id = "Start Daily Calc Task";
            timeTrigger.DaysInterval = 1;
            timeTrigger.StartBoundary =
                new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day, 0, 0, 0).ToString("s");
            timeTrigger.EndBoundary = (DateTime.Now + TimeSpan.FromDays(3650)).ToString("s");
            timeTrigger.Repetition.Interval = "P1D"; //every one day
            timeTrigger.Repetition.StopAtDurationEnd = false;

            IRegisteredTask registeredTask =
                taskFolder.RegisterTaskDefinition("Calc task", newTask, (int)_TASK_CREATION.TASK_CREATE_OR_UPDATE, null, null,
                                                 _TASK_LOGON_TYPE.TASK_LOGON_INTERACTIVE_TOKEN);
            Console.WriteLine("Task XML:\n" + registeredTask.Xml + "\n\n");
            Console.WriteLine("State: " + registeredTask.State);
            Console.WriteLine("Next run time:" + registeredTask.NextRunTime.ToLongTimeString());
            registeredTask.Run(null);
            Console.WriteLine("Press any key");
            Console.ReadKey();
            Console.WriteLine("State after execution:" + registeredTask.State);
            Console.WriteLine("Last task result:" + registeredTask.LastTaskResult);

            Console.WriteLine(registeredTask.NextRunTime.ToLongTimeString());


        }

    }
}
