namespace Powel.Icc.Services.DataContracts.HealthCheck
{
    public enum HealthCheckStatus
    {
        Unknown = 0,
        Failed = 1,
        Unhealthy = 2,
        Degraded = 3,
        Healthy = 4,
    }
}