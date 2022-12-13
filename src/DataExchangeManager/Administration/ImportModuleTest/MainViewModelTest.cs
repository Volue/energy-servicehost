using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using DataExchange.Administration.ImportModule;
using Moq;
using NUnit.Framework;
using Powel.Icc.Messaging.DataExchangeManager.DataExchangeApi;

namespace DataExchange.Administration.ImportModuleTest
{
    [TestFixture]
    public class MainViewModelTest
    {
        private Mock<IDataExchangeMessageLog> _dataExchangeMessageLogMock;
        private Mock<IDataExchangeQueueFactory> _dataExchangeQueueFactory;
        private Mock<IErroneousImportsRepository> _errorneousImportRepositoryMock;
        private TaskScheduler _currentThreadTaskScheduler;

        [SetUp]
        public void SetUp()
        {
            SynchronizationContext.SetSynchronizationContext(new SynchronizationContext());

            _dataExchangeMessageLogMock = new Mock<IDataExchangeMessageLog>();
            _dataExchangeMessageLogMock.SetupAllProperties();

            _errorneousImportRepositoryMock = new Mock<IErroneousImportsRepository>();

            _dataExchangeQueueFactory = new Mock<IDataExchangeQueueFactory>();
            _dataExchangeQueueFactory.SetupAllProperties();

            _currentThreadTaskScheduler = new CurrentThreadTaskScheduler();
        }

        private void WaitForMainViewModelToFinishAsynchronousLoading(MainViewModel mainViewModel)
        {
            var deadline = DateTime.Now.AddSeconds(1);

            while(true)
            {
                if(DateTime.Now >= deadline)
                {
                    Assert.Fail("Waiting for MainViewModel to finish loading timed out.");
                }

                if(mainViewModel.IsLoading == false)
                {
                    break;
                }

                Thread.Sleep(1);
            }
        }

        private class TestPropertyChangedListener
        {
            private readonly string propertyName;

            public bool HandleCalled { get; private set; }

            public TestPropertyChangedListener(string propertyName)
            {
                this.propertyName = propertyName;
                HandleCalled = false;
            }

            public void Handle(object sender, PropertyChangedEventArgs e)
            {
                if (e.PropertyName == this.propertyName)
                {
                    HandleCalled = true;
                }
            }
        }


        [Test]
        public void GetFailedImports_ViewModelConstructedWhenDataExchangeApiHasNoItems_FailedImportsIsNotNull()
        {
            // Assign

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(new List<FailedImportModel>());

            // Act

            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert

            Assert.IsNotNull(mainViewModel.FailedImports);
        }

        [Test]
        public void GetFailedImports_ViewModelConstructedWhenDataExchangeApiHasNoItems_FailedImportsIsEmptyCollection()
        {
            // Assign

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(new List<FailedImportModel>());

            // Act

            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert

            Assert.AreEqual(0, mainViewModel.FailedImports.Count);
        }

        [Test]
        public void GetFailedImports_ViewModelConstructedWhenDataExchangeApiHasThreeItems_ThreeFailedImportsInResult()
        {
            // Assign

            var result = new List<FailedImportModel>
                {
                    new FailedImportModel
                        {
                            InternalId = "1",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789"
                        },
                    new FailedImportModel
                        {
                            InternalId = "2",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789"
                        },
                    new FailedImportModel
                        {
                            InternalId = "3",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789"
                        }
                };

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);

            // Act
            
            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert

            Assert.AreEqual(3, mainViewModel.FailedImports.Count);
        }

        [Test]
        public void UpdateFailedImports_ItemAddedToDataExchangeApiAfterViewModelIsConstructed_CorrectNumberOfItemsInFailedImports()
        {
            // Assign

            var result = new List<FailedImportModel>
                {
                    new FailedImportModel
                        {
                            InternalId = "1",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789"
                        }
                };

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);

            // Act

            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            result.Add(new FailedImportModel
                {
                    InternalId = "2",
                    ImportProperties = new List<FailedImportProperty>
                                       {
                                           new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                           new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                       },
                    ImportMessage = "0123456789"
                });

            mainViewModel.RefreshListCommand.Execute();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert

            Assert.AreEqual(2, mainViewModel.FailedImports.Count);
        }

        [Test]
        public void SaveAndImportMessageCommand_SaveAndImportOneMessage_SaveAndImportIsCalledOnTheErronousImportRepository()
        {
            // Assign
            
            var result = new List<FailedImportModel>
                {
                    new FailedImportModel
                        {
                            InternalId = "1",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789",
                            IsSelected = true
                        }
                };

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);

            // Act

            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            mainViewModel.SaveAndImportCommand.Execute();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert
            
            _errorneousImportRepositoryMock.Verify(x => x.SaveAndImport(It.Is<FailedImportModel>(m => m.InternalId == "1")));
        }

        [Test]
        public void DeleteSelectedCommand_SaveAndImportOneMessage_DeleteIsCalledOnTheErronousImportRepository()
        {
            // Assign
            
            var result = new List<FailedImportModel>
                {
                    new FailedImportModel
                        {
                            InternalId = "1",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789",
                            IsSelected = true
                        }
                };

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);

            // Act

            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            mainViewModel.DeleteSelectedCommand.Execute();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert
            
            _errorneousImportRepositoryMock.Verify(x => x.Delete(It.Is<IList<FailedImportModel>>(l => l[0].InternalId == "1")));
        }

        [Test]
        public void FailedImportsSelectionChanged_PreviouslySelectedItemHasModifiedCountryName_ChangesAreReverted()
        {
            // Assign

            var result = new List<FailedImportModel>
                             {
                                 new FailedImportModel
                                     {
                                         InternalId = "1",
                                         ImportProperties = new List<FailedImportProperty>
                                                            {
                                                                new FailedImportProperty(FailedImportPropertyName.Country, "SWE1"),
                                                                new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS1")
                                                            },
                                         ImportMessage = "1111111111",
                                         IsSelected = true
                                     },
                                 new FailedImportModel
                                     {
                                         InternalId = "2",
                                         ImportProperties = new List<FailedImportProperty>
                                                            {
                                                                new FailedImportProperty(FailedImportPropertyName.Country, "SWE2"),
                                                                new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS2")
                                                            },
                                         ImportMessage = "2222222222"
                                     },
                                 new FailedImportModel
                                     {
                                         InternalId = "3",
                                         ImportProperties = new List<FailedImportProperty>
                                                            {
                                                                new FailedImportProperty(FailedImportPropertyName.Country, "SWE3"),
                                                                new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS3")
                                                            },
                                         ImportMessage = "3333333333"
                                     }
                             };

            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);

            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Select and modify first item in list
            FailedImportProperty failedImportProperty = mainViewModel.FailedImports[0].ImportProperties.First(x => x.PropertyName == FailedImportPropertyName.Country);
            failedImportProperty.PropertyValue = "some_new_value";
            mainViewModel.FailedImports[0].IsSelected = true;

            // Act

            // Change the selection
            mainViewModel.FailedImports[0].IsSelected = false;
            mainViewModel.FailedImports[1].IsSelected = true;
            mainViewModel.FailedImportsSelectionChanged(new ArrayList { mainViewModel.FailedImports[0] });
            
            // Assert
            Assert.AreEqual("SWE1", mainViewModel.FailedImports[0].Country);
        }


        [Test]
        public void SaveAndImportMessageCommand_ErrorneousImportRepositorySaveAndImportThrowsException_ErrorIsDefined()
        {
            // Assign
            var result = new List<FailedImportModel>
                {
                    new FailedImportModel
                        {
                            InternalId = "1",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789",
                            IsSelected = true
                        }
                };


            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);
            _errorneousImportRepositoryMock.Setup(x => x.SaveAndImport(It.IsAny<FailedImportModel>())).Throws(new Exception("test"));
            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);
            var listener = new TestPropertyChangedListener("Error");
            mainViewModel.PropertyChanged += listener.Handle;

            // Act
            mainViewModel.SaveAndImportCommand.Execute();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert
            Assert.IsTrue(listener.HandleCalled);
        }


        [Test]
        public void DeleteMessageCommand_ErrorneousImportRepositorySaveAndImportThrowsException_ErrorIsDefined()
        {
            // Assign
            var result = new List<FailedImportModel>
                {
                    new FailedImportModel
                        {
                            InternalId = "1",
                            ImportProperties = new List<FailedImportProperty>
                                               {
                                                   new FailedImportProperty(FailedImportPropertyName.Country, "SWE"),
                                                   new FailedImportProperty(FailedImportPropertyName.Protocol, "UTILTS")
                                               },
                            ImportMessage = "0123456789",
                            IsSelected = true
                        }
                };
            
            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Returns(result);
            _errorneousImportRepositoryMock.Setup(x => x.Delete(It.IsAny<IList<FailedImportModel>>())).Throws(new Exception("test"));
            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.Update();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);
            var listener = new TestPropertyChangedListener("Error");
            mainViewModel.PropertyChanged += listener.Handle;
            // Act
            mainViewModel.DeleteSelectedCommand.Execute();
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert
            Assert.IsTrue(listener.HandleCalled);
        }

        [Test]
        public void GetFailedImports_ErrorneousImportRepositoryGetAllThrowsException_ErrorIsDefined()
        {
            // Assign
            _errorneousImportRepositoryMock.Setup(x => x.GetAll()).Throws(new Exception("test"));
            var listener = new TestPropertyChangedListener("Error");

            // Act
            MainViewModel mainViewModel = new MainViewModel(_errorneousImportRepositoryMock.Object, _currentThreadTaskScheduler);
            mainViewModel.PropertyChanged += listener.Handle;
            mainViewModel.Update();
            Thread.Sleep(1000);
            WaitForMainViewModelToFinishAsynchronousLoading(mainViewModel);

            // Assert
            Assert.IsTrue(listener.HandleCalled);
        }

    }
}
