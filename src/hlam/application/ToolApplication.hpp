#pragma once

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>
#include <QObject>
#include <QPointer>
#include <QString>

#include "application/SingleInstance.hpp"

class ApplicationBuilder;
class ApplicationSettings;
class EditorContext;
class IAssetManagerPlugin;
class MainWindow;
class QApplication;
class QSettings;
class QStringList;

namespace graphics
{
class IGraphicsContext;
}

struct ParsedCommandLine
{
	bool IsPortable{false};
	bool LogDebugMessagesToConsole{false};
	QString FileName;
};

/**
*	@brief Handles program startup and shutdown
*/
class ToolApplication final : public QObject
{
	Q_OBJECT

public:
	ToolApplication();
	~ToolApplication();

	int Run(int argc, char* argv[]);

private:
	void ConfigureApplication(const QString& programName);
	
	ParsedCommandLine ParseCommandLine(const QStringList& arguments);

	std::unique_ptr<QSettings> CreateSettings(const QString& programName, bool isPortable);

	void ConfigureOpenGL(ApplicationSettings& settings);

	bool CheckSingleInstance(const QString& programName, const QString& fileName, ApplicationSettings& settings);

	std::unique_ptr<EditorContext> CreateEditorContext(std::shared_ptr<ApplicationSettings> applicationSettings,
		std::unique_ptr<graphics::IGraphicsContext>&& graphicsContext);

	bool AddPlugins(ApplicationBuilder& builder);

	std::unique_ptr<graphics::IGraphicsContext> InitializeOpenGL();

private:
	template<typename TFunction, typename... Args>
	void CallPlugins(TFunction&& function, Args&&... args)
	{
		for (auto& plugin : _plugins)
		{
			(*plugin.*function)(std::forward<Args>(args)...);
		}
	}

private slots:
	void OnExit();

	void OnFileNameReceived(const QString& fileName);

	void OnStylePathChanged(const QString& stylePath);

private:
	QApplication* _application{};

	std::vector<std::unique_ptr<IAssetManagerPlugin>> _plugins;

	std::unique_ptr<EditorContext> _editorContext;
	QPointer<MainWindow> _mainWindow;

	std::unique_ptr<SingleInstance> _singleInstance;
};
