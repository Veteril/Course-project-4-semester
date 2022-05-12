#include "Controller.h"

wsp::Controller::Controller()
	: m_pMftParsers(std::make_unique<std::vector<MFTParser>>()),
	  m_pDrivesInfo(std::make_shared<std::vector<MFTInfo>>())
{ }

BOOLEAN wsp::Controller::start()
{
	try
	{
		PartitionTableParser parser;
		parser.parse();

		try
		{
			wsp::DrivesInfo di(parser.getLogicalDrives());
			di.getDrivesInfo();
			m_pDrivesInfo = di.getDrivesMFT();

			for (auto& mftInfo : *m_pDrivesInfo)
			{
				m_pMftParsers->emplace_back(std::move(MFTParser(mftInfo)));
			}
		}
		catch (std::runtime_error error)
		{
			std::cout << error.what() << std::endl;
			return FALSE;
		}
	}
	catch (std::runtime_error error)
	{
		std::cout << error.what() << std::endl;
		return FALSE;
	}

	return TRUE;
}

const std::shared_ptr<wsp::VolumeInfo> wsp::Controller::getVolumeInfo(std::size_t volumeNumb) const
{
	return m_pMftParsers->at(volumeNumb).getVolumeInfo();
}

const std::shared_ptr<std::vector<wsp::MFTInfo>> wsp::Controller::getDrivesInfo() const
{
	return m_pDrivesInfo;
}

void wsp::Controller::findDeletedFiles(std::size_t volumeNumb)
{
	m_pMftParsers->at(volumeNumb).findDeletedFiles();
}

const std::shared_ptr<std::list<wsp::DeletedFile>> wsp::Controller::getDeletedFiles(std::size_t volumeNumb) const
{
	return m_pMftParsers->at(volumeNumb).getDeletedFiles();
}

void wsp::Controller::undelete(std::size_t volumeNumb, std::size_t fileNumb, const std::wstring& directory) const
{
	auto deletedFiles = m_pMftParsers->at(volumeNumb).getDeletedFiles();
	m_pMftParsers->at(volumeNumb).undelete(&(*std::next(deletedFiles->begin(), fileNumb)), directory.c_str());
}

std::size_t wsp::Controller::getNumberOfVolumes() const
{
	return m_pMftParsers->size();
}



