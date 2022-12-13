using System.ServiceModel;
using System.ServiceModel.Web;
using System.Threading.Tasks;
using Powel.Icc.Services.DataContracts;
using Powel.Icc.Services.DataContracts.HealthCheck;

namespace Powel.Icc.Services.ServiceContracts
{

    [ServiceContract]
    public interface IHealthCheckService
    {
        [OperationContract]
        [WebGet(
            UriTemplate = "",
            RequestFormat = WebMessageFormat.Json,
            ResponseFormat = WebMessageFormat.Json,
            BodyStyle = WebMessageBodyStyle.Bare)]
        Task<HealthCheckResult> CheckHealthAsync();
    }
}