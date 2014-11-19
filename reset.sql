
    DROP TABLE IF EXISTS HANDS;
    DROP TABLE IF EXISTS PLAYERS;
    DROP TABLE IF EXISTS GAME;

    CREATE TABLE IF NOT EXISTS HANDS(
        handID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
        pIndex INTEGER,
        context INTEGER,
        cardID INTEGER
    );

    CREATE TABLE IF NOT EXISTS PLAYERS(
        playerID INTEGER PRIMARY KEY NOT NULL, 
        points INTEGER NOT NULL,
        isHuman BOOLEAN NOT NULL,
        name TEXT,
        lastPlayed INTEGER,
        hand INTEGER, 
        scoreHand INTEGER, 
        FOREIGN KEY(scoreHand) REFERENCES HANDS(handID),
        FOREIGN KEY(hand) REFERENCES HANDS(handID) 
    ); 

    CREATE TABLE IF NOT EXISTS GAME(
        ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
        dealerPos INTEGER NOT NULL,
        pTurn INTEGER NOT NULL,
        phase INTEGER NOT NULL,
        goNumber INTEGER NOT NULL,
        cutCard INTEGER NOT NULL,
        p0 INTEGER NOT NULL,
        p1 INTEGER NOT NULL, 
        crib INTEGER,
        FOREIGN KEY(crib) REFERENCES hands(handsID),
        FOREIGN KEY (p0) REFERENCES PLAYERS(playerID), 
        FOREIGN KEY (p1) REFERENCES PLAYERS(playerID) 
    );

    INSERT INTO GAME (ID, dealerPos, pTurn, phase, goNumber, cutCard, p0, p1, crib) 
    VALUES (NULL, 0, 1, 1, 0, 0, 0, 1, 2);
    INSERT INTO PLAYERS (playerID, points, isHuman, name, lastPlayed, hand, scoreHand)
    VALUES (0, 0, 1, 'Jesus', 0, 0, 1);
    INSERT INTO PLAYERS (playerID, points, isHuman, name, lastPlayed, hand, scoreHand)
    VALUES (1, 0, 0, 'ROBOT', 0, 0, 1);
