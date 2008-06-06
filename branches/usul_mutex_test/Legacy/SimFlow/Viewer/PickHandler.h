

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public: 

    PickHandler(osgUtil::SceneView* sceneView):
        _sceneView(sceneView),
        _mx(0.0),_my(0.0) {}

    ~PickHandler() {}

    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
    {
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYUP):
            {
                if (ea.getKey()=='s')
                {
                    saveSelectedModel();
                }
                if (ea.getKey()=='p')
                {
                  SimFlow::Scene::SceneManager::instance().animate( !SimFlow::Scene::SceneManager::instance().animate() );
                }
                return false;
            }
            case(osgGA::GUIEventAdapter::PUSH):
            case(osgGA::GUIEventAdapter::MOVE):
            {
                _mx = ea.getX();
                _my = ea.getY();
                return false;
            }
            case(osgGA::GUIEventAdapter::RELEASE):
            {
                if (_mx == ea.getX() && _my == ea.getY())
                {
                    // only do a pick if the mouse hasn't moved
                    pick(ea);
                }
                return true;
            }    

            default:
                return false;
        }
    }

    void pick(const osgGA::GUIEventAdapter& ea)
    {

        osg::Node* scene = _sceneView.valid() ? _sceneView->getSceneData() : 0;
        if (!scene) return;

        // remap the mouse x,y into viewport coordinates.
        
        float mx = _sceneView->getViewport()->x() + (int)((float)_sceneView->getViewport()->width()*(ea.getXnormalized()*0.5f+0.5f));
        float my = _sceneView->getViewport()->y() + (int)((float)_sceneView->getViewport()->height()*(ea.getYnormalized()*0.5f+0.5f));
   
        // do the pick traversal.
        osgUtil::PickVisitor pick(_sceneView->getViewport(),
                                  _sceneView->getProjectionMatrix(), 
                                  _sceneView->getViewMatrix(), mx, my);
        scene->accept(pick);

        osgUtil::PickVisitor::LineSegmentHitListMap& segHitList = pick.getSegHitList();
        if (!segHitList.empty() && !segHitList.begin()->second.empty())
        {
            std::cout<<"Got hits"<<std::endl;

            // get the hits for the first segment
            osgUtil::PickVisitor::HitList& hits = segHitList.begin()->second;

            // just take the first hit - nearest the eye point.
            osgUtil::Hit& hit = hits.front();

            osg::NodePath& nodePath = hit._nodePath;
            osg::Node* node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;
            osg::Group* parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;

            if (node) std::cout<<"  Hits "<<node->className()<<" nodePath size"<<nodePath.size()<<std::endl;

            // now we try to decorate the hit node by the osgFX::Scribe to show that its been "picked"
            if (parent && node)
            {

                std::cout<<"  parent "<<parent->className()<<std::endl;

                osgFX::Scribe* parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent);
                if (!parentAsScribe)
                {
                    // node not already picked, so highlight it with an osgFX::Scribe
                    osgFX::Scribe* scribe = new osgFX::Scribe();
                    scribe->addChild(node);
                    parent->replaceChild(node,scribe);
                }
                else
                {
                    // node already picked so we want to remove scribe to unpick it.
                    osg::Node::ParentList parentList = parentAsScribe->getParents();
                    for(osg::Node::ParentList::iterator itr=parentList.begin();
                        itr!=parentList.end();
                        ++itr)
                    {
                        (*itr)->replaceChild(parentAsScribe,node);
                    }
                }
            }

        }
    }

    void saveSelectedModel()
    {
        CreateModelToSaveVisitor cmtsv;
        _sceneView->getSceneData()->accept(cmtsv);
        
        osgDB::writeNodeFile( ( * _sceneView->getSceneData() ), "Scene2.osg" );

        if (cmtsv._group->getNumChildren()>0)
        {
            std::cout<<"Writing selected compoents to 'selected_model.osg'"<<std::endl;
            osgDB::writeNodeFile(*cmtsv._group, "selected_model.osg");
        }
    }

protected:

    osg::observer_ptr<osgUtil::SceneView> _sceneView;
    float _mx,_my;
};